
#include "Renderer/Vulkan/VulkanTexture.h"

#include "VulkanUtils.h"
#include "Renderer/Vulkan/VulkanRenderer.h"

// Created on 2026-01-30 by sisco

namespace pdl
{
    vk::ImageView VulkanTexture::CreateView(VulkanRenderer& renderer, vk::ImageViewType viewType, vk::Format format,
        vk::ImageAspectFlags aspectFlags, uint32 baseMip, uint32 numMips, uint32 baseLayer, uint32 numLayers,
        vk::ComponentMapping componentMapping) const
    {
        vk::ImageViewCreateInfo createInfo(
            {},
            m_vkImage,
            viewType,
            format,
            componentMapping,
            vk::ImageSubresourceRange(aspectFlags, baseMip, numMips, baseLayer, numLayers));
        
        auto createImageViewResults = renderer.GetDevice().createImageView(createInfo);
        CHECK_VK_RESULTVALUE(createImageViewResults);
        return createImageViewResults.value;
    }
}

