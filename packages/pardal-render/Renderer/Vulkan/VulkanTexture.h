
#pragma once
#include <vulkan/vulkan.hpp>
#include <vma/vk_mem_alloc.h>

#include "VulkanCommandBuffer.h"
#include "Base/BaseDefines.h"
#include "Base/BaseTypes.h"
// Created on 2026-01-30 by sisco

namespace pdl
{
class VulkanRenderer;

struct VulkanTexture
{
    enum class Flags : uint32
    {
        OwnsVkImage = 1 << 0,
        IsResolveAttachment = 1 << 1
        
    };
    
    vk::Image m_vkImage;
    vk::ImageUsageFlags m_vkUsage;
    vk::Format m_vkFormat = vk::Format::eUndefined;
    vk::Extent3D m_vkExtent {0, 0, 0};
    uint32 m_mipLevels = 0;
    uint32 m_arrayLayers = 0;
    vk::SampleCountFlags m_vkSamples = vk::SampleCountFlagBits::e1;
    vk::ImageLayout m_vkLayout = vk::ImageLayout::eUndefined;

    vk::ImageView m_vkImageView;
    Array<Array<vk::ImageView, 6>, RenderererConstants::MaxMipLevels()> m_vkFramebufferImageViews;
    
    Flags m_flags = {};
    
    VmaAllocation m_vmaAllocation;
    void* m_mappedPtr = nullptr;
    

    vk::ImageView CreateView(VulkanRenderer& renderer,
        vk::ImageViewType viewType,
        vk::Format format,
        vk::ImageAspectFlags aspectFlags,
        uint32 baseMip,
        uint32 numMips = vk::RemainingMipLevels,
        uint32 baseLayer = 0,
        uint32 numLayers = 1,
        vk::ComponentMapping componentMapping = {}) const;
    
    void CreateDefaultView(VulkanRenderer& renderer,
        vk::ImageViewType viewType,
        vk::Format format,
        vk::ImageAspectFlags aspectFlags,
        uint32 baseMip,
        uint32 numMips = vk::RemainingMipLevels,
        uint32 baseLayer = 0,
        uint32 numLayers = 1,
        vk::ComponentMapping componentMapping = {})
    {
        m_vkImageView = CreateView(renderer, viewType, format, aspectFlags, baseMip, numMips, baseLayer, numLayers, componentMapping);
    }
    
    vk::ImageAspectFlags GetAspectFlags() const;
    void TransitionLayout(vk::CommandBuffer commandBuffer, vk::ImageLayout newLayout);
    vk::ImageView& GetOrCreateVkImageViewForFramebuffer(VulkanRenderer& vulkanRenderer, uint8 level, uint8 layer);
};
    
DefineEnumMaskOperators(VulkanTexture::Flags);

}

