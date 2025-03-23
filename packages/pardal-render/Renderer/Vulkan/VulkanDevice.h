
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
protected:
    bool Initialize(const InitInfoBase& initInfo) override;
    bool InitializeInstanceAndDevice(const InitInfoBase& initInfo);
    
    RenderDeviceInfo m_deviceInfo = {};

    vk::Instance m_vkInstance;
    vk::PhysicalDevice m_vkPhysicalDevice;
    vk::Device m_vkDevice;
    vk::Sampler m_vkDefaultSampler;
    vk::DebugUtilsMessengerEXT m_vkDebugMessenger;
    VulkanDeviceQueue m_vulkanDeviceQueue;
};

}

