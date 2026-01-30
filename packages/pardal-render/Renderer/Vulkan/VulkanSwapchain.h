
#pragma once
#include "Base/BaseTypes.h"
#include "Base/Expected.h"
#include "String/String.h"
#include <vulkan/vulkan.hpp>

#include "Base/BaseDefines.h"
#include "Renderer/RendererTypes.h"

// Created on 2026-01-30 by sisco

namespace pdl
{
class VulkanRenderer;

class VulkanSwapchain
{
public:
    VulkanSwapchain(VulkanRenderer& renderer, uint32 width, uint32 height, ColorSpace colorSpace);
    ~VulkanSwapchain();
    
    Expected<void, StringView> Present(vk::Semaphore semaphore);
    
    TextureHandle GetCurrentImage() const { return m_swapchainImages[m_currentImageIndex]; }
    uint32 GetCurrentImageIndex() const { return m_currentImageIndex; }
    uint64 GetCurrentFrameIndex() const { return m_currentFrameIndex; }
    

private:
    static constexpr uint32 kMaxSwapchainImages = 16;
    
    VulkanRenderer& m_renderer;
    vk::SwapchainKHR m_swapchain;
    vk::SurfaceFormatKHR m_swapchainFormat;
    vk::Extent2D m_swapchainExtent;
    uint32 m_swapchainImageCount = 0;
    uint32 m_currentImageIndex = 0;
    uint64 m_currentFrameIndex = 0;
    
    // Sync objects
    vk::Semaphore m_acquireSemaphore[kMaxSwapchainImages];
    vk::Fence m_presentFence[kMaxSwapchainImages];
    vk::Fence m_acquireFence[kMaxSwapchainImages];
    
    // Resources
    TextureHandle m_swapchainImages[kMaxSwapchainImages];
};

}

