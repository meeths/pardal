
#include "Renderer/Vulkan/VulkanBindlessDescriptors.h"

#include "VulkanTextureView.h"
#include "Renderer/Vulkan/VulkanDevice.h"
#include "Renderer/Vulkan/VulkanUtils.h"

// Created on 2025-05-04 by sisco

namespace pdl
{
    VulkanBindlessDescriptors::VulkanBindlessDescriptors(const VulkanDevice& _device) : m_device(_device.GetVkDevice())
    {
    }

    bool VulkanBindlessDescriptors::Initialize(BindlessDescriptorInfo& info)
    {
        Vector<vk::DescriptorSetLayoutBinding> layoutBindings(info.m_descriptorTypes.size());
        Vector<vk::DescriptorBindingFlags> bindingFlags(info.m_descriptorTypes.size());
        Vector<vk::DescriptorPoolSize> poolSizes(info.m_descriptorTypes.size());
        for (uint32 i = 0; i < info.m_descriptorTypes.size(); ++i)
        {
            layoutBindings[i].binding = i;
            layoutBindings[i].descriptorType = VulkanUtils::GetDescriptorType(info.m_descriptorTypes[i]);
            layoutBindings[i].descriptorCount = info.m_descriptorCount;
            layoutBindings[i].stageFlags = vk::ShaderStageFlagBits::eAll;
            bindingFlags[i] = vk::DescriptorBindingFlagBits::ePartiallyBound | vk::DescriptorBindingFlagBits::eUpdateAfterBind;
            poolSizes[i].type = layoutBindings[i].descriptorType;
            poolSizes[i].descriptorCount = info.m_descriptorCount;
            switch (info.m_descriptorTypes[i])
            {
                case DescriptorTypes::UniformBuffer:
                    pdlAssert(m_uniformBindingIndex == kInvalidBindingIndex);
                    m_uniformBindingIndex = i;
                    break;
                case DescriptorTypes::StorageBuffer:
                    pdlAssert(m_storageBindingIndex == kInvalidBindingIndex);
                    m_storageBindingIndex = i;
                    break;
                case DescriptorTypes::CombinedSampler:
                    pdlAssert(m_textureBindingIndex == kInvalidBindingIndex);
                    m_textureBindingIndex = i;
                    break;
                case DescriptorTypes::AccelerationStructure:
                    pdlAssert(m_accelerationStructureBindingIndex == kInvalidBindingIndex);
                    m_accelerationStructureBindingIndex = i;
                    break;
                default:
                    pdlNotImplemented();
            }
        }

        vk::DescriptorSetLayoutBindingFlagsCreateInfo bindingFlagsInfo {};
        bindingFlagsInfo.bindingCount = static_cast<uint32>(bindingFlags.size());
        bindingFlagsInfo.pBindingFlags = bindingFlags.data();

        vk::DescriptorSetLayoutCreateInfo layoutInfo {};
        layoutInfo.bindingCount = static_cast<uint32>(layoutBindings.size());
        layoutInfo.pBindings = layoutBindings.data();
        layoutInfo.flags = vk::DescriptorSetLayoutCreateFlagBits::eUpdateAfterBindPool;
        layoutInfo.pNext = &bindingFlagsInfo;

        auto descriptorLayoutResults = m_device.createDescriptorSetLayout(layoutInfo);
        CHECK_VK_RESULT(descriptorLayoutResults.result);
        m_bindlessLayout = descriptorLayoutResults.value;

        vk::DescriptorPoolCreateInfo poolInfo {};
        poolInfo.maxSets = info.m_descriptorCount;
        poolInfo.poolSizeCount = static_cast<uint32>(poolSizes.size());
        poolInfo.pPoolSizes = poolSizes.data();
        poolInfo.flags = vk::DescriptorPoolCreateFlagBits::eUpdateAfterBind;

        auto descriptorPoolResults = m_device.createDescriptorPool(poolInfo);
        CHECK_VK_RESULT(descriptorPoolResults.result);
        m_bindlessPool = descriptorPoolResults.value;
        
        vk::DescriptorSetAllocateInfo allocInfo {};
        allocInfo.descriptorPool = m_bindlessPool;
        allocInfo.descriptorSetCount = 1;
        allocInfo.pSetLayouts = &m_bindlessLayout;

        auto descriptorSetResults = m_device.allocateDescriptorSets(allocInfo);
        CHECK_VK_RESULT(descriptorSetResults.result);
        m_bindlessSet = descriptorSetResults.value[0];
        
        return true;
    }

    TextureDescriptorHandle VulkanBindlessDescriptors::StoreTexture(ITextureView* textureView)
    {
        pdlAssert(textureView != nullptr);
        // Bindless descriptor was not initialized with support for textures
        pdlAssert(m_textureBindingIndex != kInvalidBindingIndex);
        
        TextureDescriptorHandle nextHandle = TextureDescriptorHandle::Invalid;
        
        if (!m_freeTextureHandles.empty())
        {
            nextHandle = m_freeTextureHandles.back();
            m_freeTextureHandles.pop_back();
            pdlAssert(m_textureViews.size() > static_cast<size_t>(nextHandle));
            m_textureViews[static_cast<size_t>(nextHandle)] = textureView;
        }
        else
        {
            nextHandle = static_cast<TextureDescriptorHandle>(m_textureViews.size());
            m_textureViews.push_back(textureView);
        }
        
        pdlAssert(nextHandle != TextureDescriptorHandle::Invalid);

        auto vulkanTextureView = static_cast<VulkanTextureView*>(textureView);
        pdlAssert((vulkanTextureView->GetTexture()->GetDescriptor().m_textureUsage & TextureUsage::ShaderResource) != TextureUsage::None);
        
        vk::DescriptorImageInfo imageInfo{};
        imageInfo.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
        imageInfo.imageView = vulkanTextureView->GetVkImageView();
        imageInfo.sampler = vulkanTextureView->GetVkSampler();
        
        vk::WriteDescriptorSet writeDescriptorSet {};
        writeDescriptorSet.dstSet = m_bindlessSet;
        writeDescriptorSet.dstBinding = m_textureBindingIndex;
        writeDescriptorSet.dstArrayElement = static_cast<uint32>(nextHandle);
        writeDescriptorSet.descriptorType = VulkanUtils::GetDescriptorType(DescriptorTypes::CombinedSampler);
        writeDescriptorSet.descriptorCount = 1;
        writeDescriptorSet.pImageInfo = &imageInfo;
        
        m_pendingWrites.push_back(writeDescriptorSet);
        WriteDescriptors();
        return nextHandle;
    }

    BufferDescriptorHandle VulkanBindlessDescriptors::StoreBuffer(IRenderBuffer* buffer)
    {
        pdlAssert(buffer != nullptr);
        // Bindless descriptor was not initialized with support for buffers
        pdlAssert(m_storageBindingIndex != kInvalidBindingIndex);
        
        BufferDescriptorHandle nextHandle = BufferDescriptorHandle::Invalid;
        
        if (!m_freeBufferHandles.empty())
        {
            nextHandle = m_freeBufferHandles.back();
            m_freeBufferHandles.pop_back();
            pdlAssert(m_buffers.size() > static_cast<size_t>(nextHandle));
            m_buffers[static_cast<size_t>(nextHandle)] = buffer;
        }
        else
        {
            nextHandle = static_cast<BufferDescriptorHandle>(m_buffers.size());
            m_buffers.push_back(buffer);
        }
        
        pdlAssert(nextHandle != BufferDescriptorHandle::Invalid);
        return nextHandle;
    }

    void VulkanBindlessDescriptors::WriteDescriptors()
    {
        if (m_pendingWrites.empty())
        {
            return;
        }
        
        m_device.updateDescriptorSets(
            static_cast<uint32>(m_pendingWrites.size()),
            m_pendingWrites.data(),
            0, nullptr);
        m_pendingWrites.clear();
    }

    bool VulkanBindlessDescriptors::Bind(ICommandBuffer* commandBuffer)
    {
        pdlNotImplemented();
        //auto vkCommandBuffer = static_cast<VulkanCommandBuffer*>(commandBuffer)->GetVkCommandBuffer();
        //auto bindPoint = vk::PipelineBindPoint::eGraphics;
        //vkCommandBuffer.bindDescriptorSets(bindPoint, m_bindlessLayout, 0, 1, &m_bindlessSet, 0, nullptr);
        return false;
        
    }
}

