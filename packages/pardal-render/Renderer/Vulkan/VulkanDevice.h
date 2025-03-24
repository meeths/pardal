
#pragma once
#include <Renderer/IRenderDevice.h>
#include <vulkan/vulkan.hpp>
#include <Renderer/Vulkan/VulkanDeviceQueue.h>

// Created on 2025-03-23 by sisco

namespace pdl
{

class VulkanDevice : public IRenderDevice
{
public:
    ~VulkanDevice() override = default;
    const RenderDeviceInfo& GetRenderDeviceInfo() const override { return m_deviceInfo; }
    void WaitForGPU() override;
    
    Expected<SharedPointer<ISurface>,StringView> CreateSurface(ApplicationWindow& applicationWindow) override;
    Expected<SharedPointer<ITexture>, StringView> CreateTexture(ITexture::TextureDescriptor _textureDescriptor) override;
    
protected:
    bool Initialize(const InitInfoBase& initInfo) override;
    bool InitializeInstanceAndDevice(const InitInfoBase& initInfo);


protected:
    RenderDeviceInfo m_deviceInfo = {};

    vk::Instance m_vkInstance;
    vk::PhysicalDevice m_vkPhysicalDevice;
    vk::Device m_vkDevice;
    vk::Sampler m_vkDefaultSampler;
    vk::DebugUtilsMessengerEXT m_vkDebugMessenger;
    VulkanDeviceQueue m_vulkanDeviceQueue;
};

}

