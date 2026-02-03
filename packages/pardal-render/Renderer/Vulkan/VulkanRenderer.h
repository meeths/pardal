
#pragma once
#include "Base/BaseDefines.h"
#include "Base/Expected.h"
#include "lvk/LVK.h"
#include "Renderer/IRenderer.h"
#include "Renderer/RenderererInfo.h"


// Created on 2026-01-26 by sisco

namespace lvk
{
    class ImGuiRenderer;
    class VulkanContext;
}

namespace pdl
{
    class ImGuiRenderer;
    class VulkanImmediateCommand;
    class VulkanSwapchain;

class VulkanRenderer : public IRenderer
{
public:

    VulkanRenderer(const InitInfo& initInfo);
    ~VulkanRenderer() override;

    DeclareNonCopyable(VulkanRenderer);
    
    const RenderDeviceInfo& GetDeviceInfo() const override { return m_deviceInfo; }

    Expected<void, StringView> InitSwapchain(uint32 width, uint32 height) override;

    lvk::IContext* GetLVKContext() const;
private:
    Expected<void, StringView> InitializeInstanceAndDevice(const InitInfo& initInfo);
    RenderDeviceInfo m_deviceInfo = {};
    
    UniquePointer<lvk::VulkanContext> m_context;
#ifdef PDL_FEATURE_IMGUI
    UniquePointer<ImGuiRenderer>  m_imGuiRenderer;
#endif    
};

}

