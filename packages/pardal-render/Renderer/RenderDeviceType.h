
#pragma once
#include <Base/BaseTypes.h>

// Created on 2025-03-23 by sisco

namespace pdl
{

    enum class RenderDeviceType : uint8
    {
        None,
        D3D11,
        D3D12,
        Metal,
        Vulkan,
        OpenGL,
    };

    inline const char* to_string(RenderDeviceType e)
    {
        switch (e)
        {
        case RenderDeviceType::None: return "None";
        case RenderDeviceType::D3D11: return "D3D11";
        case RenderDeviceType::D3D12: return "D3D12";
        case RenderDeviceType::Metal: return "Metal";
        case RenderDeviceType::Vulkan: return "Vulkan";
        case RenderDeviceType::OpenGL: return "OpenGL";
        default: return "unknown";
        }
    }


}

