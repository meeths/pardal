
#pragma once
#include "Renderer/RendererTypes.h"
#include "Base/BaseTypes.h"
#include "String/String.h"

// Created on 2026-01-26 by sisco

namespace pdl
{
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
        String m_pipelineCachePath;
    };
    
    virtual ~IRenderer() = default;
    
    virtual const RenderDeviceInfo& GetDeviceInfo() const = 0;
    
    virtual Expected<void, StringView> InitSwapchain(uint32 width, uint32 height) = 0;
    
    virtual Expected<BufferHandle, StringView> CreateBuffer(uint32 size, 
        BufferUsage usage,
        MemoryType memoryType) = 0;
    
    virtual void Destroy(TextureHandle bufferHandle) = 0;
    virtual void Destroy(BufferHandle bufferHandle) = 0;
};

}

