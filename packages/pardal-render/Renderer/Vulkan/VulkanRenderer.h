
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
};

}

