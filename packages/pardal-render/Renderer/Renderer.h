
#pragma once
#include <Memory/SharedPointer.h>
#include <Renderer/IRenderDevice.h>

// Created on 2025-03-23 by sisco

namespace pdl
{

    class Renderer
    {
    public:
        bool InitializeRenderDevice(IRenderDevice::InitInfoBase deviceDescriptor);
        SharedPointer<IRenderDevice> GetRenderDevice() const { return m_device; }
    private:
        SharedPointer<IRenderDevice> m_device;
    };

}