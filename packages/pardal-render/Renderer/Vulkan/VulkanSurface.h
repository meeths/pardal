
#pragma once
#include <Renderer/ISurface.h>
#include <vulkan/vulkan.hpp>
#include <Renderer/Vulkan/VulkanTexture.h>

// Created on 2025-03-23 by sisco

namespace pdl
{
class ApplicationWindow;

class VulkanSurface : public ISurface
{
public:
    VulkanSurface() = default;
    ~VulkanSurface() override;
    
    bool Initialize(vk::Device* device, vk::PhysicalDevice* physicalDevice, vk::Instance* instance, ApplicationWindow& windowHandle, Format preferredFormat);
    bool CreateSwapchain();
    void DestroySwapchain();
    
    const SurfaceInfo& GetSurfaceInfo() const override { return m_info; }
    const SwapchainDescriptor& GetSurfaceConfig() const override { return m_config; }
    size_t GetImageCount() const override { return m_images.size(); } 
    bool ConfigureSwapchain(SwapchainDescriptor config) override;
    ITexture* GetTexture() override;
    bool Present() override;
private:
    SurfaceInfo m_info {};
    SwapchainDescriptor m_config = {};
    
    vk::Device* m_vkDevice = nullptr;
    vk::PhysicalDevice* m_vkPhysicalDevice = nullptr;
    vk::Instance* m_vkInstance = nullptr;
    vk::SwapchainKHR m_vkSwapchain;
    vk::SurfaceKHR m_vkSurface;
    vk::Semaphore m_vkNextImageAcquireSemaphore;
    vk::Format m_vkFormat = {};
    Vector<VulkanTexture> m_images;
    size_t m_currentImageIndex = 0;
};

}

