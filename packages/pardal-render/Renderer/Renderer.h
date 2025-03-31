
#pragma once
#include <Memory/SharedPointer.h>
#include <Renderer/IRenderDevice.h>

// Created on 2025-03-23 by sisco

namespace pdl
{
    class RenderPass;
    class IInternalRenderer;

class Renderer
{
public:
    bool InitializeRenderDevice(const IRenderDevice::InitInfoBase& initInfo);
    SharedPointer<IRenderDevice> GetRenderDevice() const { return m_device; }

    bool BeginFrame();
    bool EndFrame();

    RenderPass& GetMainRenderPass();
    bool BeginRenderPass(const RenderPass& renderPass);
    bool EndRenderPass();

private:
    SharedPointer<IRenderDevice> m_device;
    SharedPointer<IInternalRenderer> m_internalRenderer;
};

}