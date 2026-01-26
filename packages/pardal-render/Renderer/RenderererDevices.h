
#pragma once
#include <Base/BaseTypes.h>
#include <String/StringCast.h>

#include "Memory/UniquePointer.h"

// Created on 2026-01-26 by sisco


namespace pdl
{
    class VulkanRenderer;
    class IRenderer;

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

    template <>
    inline RenderDeviceType StringCast::FromString<RenderDeviceType>(const StringView& valueStr)
    {   
        if (valueStr == "None") return RenderDeviceType::None;
        if (valueStr == "D3D11") return RenderDeviceType::D3D11;
        if (valueStr == "D3D12") return RenderDeviceType::D3D12;
        if (valueStr == "Metal") return RenderDeviceType::Metal;
        if (valueStr == "Vulkan") return RenderDeviceType::Vulkan;
        if (valueStr == "OpenGL") return RenderDeviceType::OpenGL;
        return RenderDeviceType::None;
    }

    template<typename... Args>
    static UniquePointer<IRenderer> CreateRenderer(RenderDeviceType type, Args && ...args)
    {
        switch (type) {
        case RenderDeviceType::Vulkan:
            return MakeUniquePointer<VulkanRenderer>(std::forward<Args>(args)...);
        case RenderDeviceType::None:
        case RenderDeviceType::D3D11:
        case RenderDeviceType::D3D12:
        case RenderDeviceType::Metal:
        case RenderDeviceType::OpenGL:
            pdlNotImplemented();
            return nullptr;
        }
        
    }

}

