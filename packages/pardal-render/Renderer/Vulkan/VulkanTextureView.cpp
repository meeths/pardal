
#include <Renderer/Vulkan/VulkanTextureView.h>

#include "VulkanTexture.h"
#include "VulkanUtils.h"

// Created on 2025-03-26 by sisco

namespace pdl
{
    vk::Sampler VulkanTextureView::m_defaultSampler = VK_NULL_HANDLE;

    vk::Sampler CreateDefaultSampler(vk::Device* device)
    {
        vk::SamplerCreateInfo samplerInfo;
        samplerInfo.magFilter = vk::Filter::eLinear;
        samplerInfo.minFilter = vk::Filter::eLinear;
        samplerInfo.mipmapMode = vk::SamplerMipmapMode::eLinear;
        samplerInfo.addressModeU = vk::SamplerAddressMode::eClampToEdge;
        samplerInfo.addressModeV = vk::SamplerAddressMode::eClampToEdge;
        samplerInfo.addressModeW = vk::SamplerAddressMode::eClampToEdge;
        samplerInfo.anisotropyEnable = VK_FALSE;
        samplerInfo.maxAnisotropy = 16;
        samplerInfo.borderColor = vk::BorderColor::eFloatOpaqueWhite;
        samplerInfo.unnormalizedCoordinates = VK_FALSE;
        samplerInfo.compareEnable = VK_FALSE;
        samplerInfo.compareOp = vk::CompareOp::eAlways;
        samplerInfo.mipLodBias = 0.0f;
        samplerInfo.minLod = 0.0f;
        samplerInfo.maxLod = 0.0f;
        auto createSamplerResults = device->createSampler(samplerInfo);
        CHECK_VK_RESULTVALUE(createSamplerResults);
        return createSamplerResults.value;
    }
    
    VulkanTextureView::VulkanTextureView(const TextureViewDescriptor& desc, vk::Device* device)
        : m_descriptor(desc), m_device(device)
    {

        if (m_defaultSampler == VK_NULL_HANDLE)
        {
            m_defaultSampler = CreateDefaultSampler(device);
        }
        
        bool initResults = Initialize();
        m_sampler = m_defaultSampler;
        pdlAssert(initResults);
    }

    VulkanTextureView::~VulkanTextureView()
    {
        if (m_imageView != VK_NULL_HANDLE)
        {
            m_device->destroyImageView(m_imageView);
        }
    }

    bool VulkanTextureView::Initialize()
    {
        VulkanTexture* vulkanImage = static_cast<VulkanTexture*>(m_descriptor.m_texture);
        const auto vulkanTextureDescriptor = vulkanImage->GetDescriptor();
        vk::ImageViewCreateInfo createInfo;
        createInfo.image = vulkanImage->GetVkImage();
        switch (vulkanTextureDescriptor.m_textureType)
        {
        case TextureType::Texture1D:
            createInfo.viewType = vk::ImageViewType::e1D;
            createInfo.subresourceRange.layerCount = 1;
            break;
        case TextureType::Texture2D:
            createInfo.viewType = vk::ImageViewType::e2D;
            createInfo.subresourceRange.layerCount = 1;
            break;
        case TextureType::Texture3D:
            createInfo.viewType = vk::ImageViewType::e3D;
            createInfo.subresourceRange.layerCount = 1;
            break;
        case TextureType::TextureCube:
            createInfo.viewType = vk::ImageViewType::eCube;
            createInfo.subresourceRange.layerCount = 6;
            break;
        default:
            pdlLogError("Invalid texture type");
            return false;
        }
        
        createInfo.format = vulkanImage->GetVkFormat();
        createInfo.components.r = vk::ComponentSwizzle::eR;
        createInfo.components.g = vk::ComponentSwizzle::eG;
        createInfo.components.b = vk::ComponentSwizzle::eB;
        createInfo.components.a = vk::ComponentSwizzle::eA;
        createInfo.subresourceRange.aspectMask = VulkanUtils::GetVkAspectFlagsFromFormat(createInfo.format);
        createInfo.subresourceRange.baseMipLevel = m_descriptor.m_baseMipLevel;
        createInfo.subresourceRange.levelCount = vulkanTextureDescriptor.m_mipLevels;
       
        auto createImageViewResults = m_device->createImageView(createInfo);
        CHECK_VK_RESULTVALUE(createImageViewResults);
        m_imageView = createImageViewResults.value;

        return true;
    }
}

