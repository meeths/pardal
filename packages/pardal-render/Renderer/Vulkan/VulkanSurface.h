
#pragma once
#include <Renderer/ISurface.h>
#include <vulkan/vulkan.hpp>
#include <Renderer/Vulkan/VulkanTexture.h>
#include <Renderer/Vulkan/VulkanTextureView.h>

#include "Memory/UniquePointer.h"

// Created on 2025-03-23 by sisco

namespace pdl
{
class VulkanDeviceQueue;
class ApplicationWindow;

class VulkanSurface : public ISurface
{
public:
    VulkanSurface() = default;
    ~VulkanSurface() override;
    
    bool Initialize(vk::Device* device, vk::PhysicalDevice* physicalDevice, vk::Instance* instance, const VulkanDeviceQueue& deviceQueue, ApplicationWindow& windowHandle, Format preferredFormat);
    bool CreateSwapchain();
    void DestroySwapchain();
    
    const SurfaceInfo& GetSurfaceInfo() const override { return m_info; }
    const SwapchainDescriptor& GetSurfaceConfig() const override { return m_config; }
    size_t GetImageCount() const override { return m_images.size(); }
    
    bool ConfigureSwapchain(SwapchainDescriptor config) override;
    ITextureView* GetCurrentTextureView() override;

    bool BeginFrame() override;
    bool Present() override;
private:
    void AcquireNextImage();


private:
    SurfaceInfo m_info {};
    SwapchainDescriptor m_config = {};
    
    vk::Device* m_vkDevice = nullptr;
    vk::PhysicalDevice* m_vkPhysicalDevice = nullptr;
    vk::Instance* m_vkInstance = nullptr;
    vk::SwapchainKHR m_vkSwapchain;
    vk::SurfaceKHR m_vkSurface;
    vk::Semaphore m_vkNextImageAcquireSemaphore;
    vk::Semaphore m_vkEndFrameSemaphore;
    vk::Format m_vkFormat = {};
    vk::Queue m_vkPresentQueue;
    Vector<UniquePointer<VulkanTexture>> m_images;
    Vector<UniquePointer<VulkanTextureView>> m_imageViews;
    uint32 m_currentImageIndex = 0;

    bool m_needToRecreateSwapChain = false;
};

}

