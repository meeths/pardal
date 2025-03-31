
#pragma once
#include <Renderer/RendererTypes.h>
#include "Containers/Vector.h"
#include <Math/Vector2i.h>

// Created on 2025-03-23 by sisco

namespace pdl
{
    class ITexture;
    class ITextureView;
    
class ISurface
{
public:
    struct SurfaceInfo
    {
        Format m_preferredFormat;
        TextureUsage m_supportedUsage;
        Vector<Format> m_supportedFormats;
    };    

    struct SwapchainDescriptor
    {
        Format m_format = Format::Unknown;
        Math::Vector2i m_size = Math::Vector2i(0);
        bool m_vsync = true;
        uint32_t m_desiredImageCount = 3;
        TextureUsage m_usage = TextureUsage::RenderTarget;
    };

    virtual ~ISurface() = default;
    virtual const SurfaceInfo& GetSurfaceInfo()  const= 0;
    virtual const SwapchainDescriptor& GetSurfaceConfig() const = 0;
    virtual size_t GetImageCount() const = 0; 

    virtual bool ConfigureSwapchain(SwapchainDescriptor config) = 0;
    virtual ITextureView* GetCurrentTextureView() = 0;

    virtual bool BeginFrame() = 0;
    virtual bool Present() = 0;
};

}

