
#include <Renderer/Vulkan/VulkanTextureView.h>

#include "VulkanTexture.h"
#include "VulkanUtils.h"

// Created on 2025-03-26 by sisco

namespace pdl
{
    VulkanTextureView::VulkanTextureView(const TextureViewDescriptor& desc, vk::Device* device)
        : m_descriptor(desc), m_device(device)
    {
        bool initResults = Initialize();
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

