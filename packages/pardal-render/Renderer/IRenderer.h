
#pragma once
#include "Renderer/RendererTypes.h"
#include "Base/BaseTypes.h"
#include "String/String.h"

// Created on 2026-01-26 by sisco

namespace pdl
{
    class ICommandBuffer;
    struct RenderDeviceInfo;
    class ApplicationWindow;

class IRenderer
{
public:
    struct InitInfo
    {
        StringView m_applicationName;
        ApplicationWindow& m_applicationWindow;
        bool m_enableValidation = true;
        bool m_useVSync = true;
        bool m_useHDR = false;
        bool m_useLinearColorSpace = false;
        String m_pipelineCachePath;
        int m_preferredDeviceIndex = -1;
    };
    
    virtual ~IRenderer() = default;
    
    virtual const RenderDeviceInfo& GetDeviceInfo() const = 0;
    
    virtual Expected<void, StringView> InitSwapchain(uint32 width, uint32 height) = 0;
    

};

}

