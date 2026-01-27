
#pragma once
#include "Base/BaseDefines.h"
#include "Base/Expected.h"
#include "Renderer/IRenderer.h"
#include "Renderer/RenderererInfo.h"
#include <vulkan/vulkan.hpp>

// Created on 2026-01-26 by sisco

namespace pdl
{

class VulkanRenderer : public IRenderer
{
public:
    VulkanRenderer(const InitInfo& initInfo);
    ~VulkanRenderer() override;

    DeclareNonCopyable(VulkanRenderer);
    
    const RenderDeviceInfo& GetDeviceInfo() const override { return m_deviceInfo; }
private:
    Expected<void, StringView> InitializeInstanceAndDevice(const InitInfo& initInfo);
    RenderDeviceInfo m_deviceInfo = {};
    
    vk::Instance m_vkInstance;
    vk::PhysicalDevice m_vkPhysicalDevice;
    vk::Device m_vkDevice;
    vk::DebugUtilsMessengerEXT m_vkDebugMessenger;
    vk::SurfaceKHR m_vkSurface;
    
    vk::PipelineCache m_vkPipelineCache;
    struct DeviceQueues 
    {
        static constexpr int32 INVALID = -1;
        int32 graphicsQueueFamilyIndex = INVALID;
        int32 computeQueueFamilyIndex = INVALID;

        vk::Queue graphicsQueue;
        vk::Queue computeQueue;
    };
    
    DeviceQueues m_deviceQueues;
};

}

