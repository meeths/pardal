
#pragma once
#include <String/String.h>
#include <Renderer/RenderDeviceInfo.h>

// Created on 2025-03-23 by sisco

namespace pdl
{

class IRenderDevice
{
    friend class Renderer;
public:
   
    struct InitInfoBase
    {
        RenderDeviceType deviceType = RenderDeviceType::None;
        StringView applicationName;
        bool enableValidation = true;
    };

    virtual ~IRenderDevice() = default;
    virtual const RenderDeviceInfo& GetRenderDeviceInfo() const = 0;

    virtual void WaitForGPU() = 0;

protected:
    virtual bool Initialize(const InitInfoBase& desc) = 0;
};

}

