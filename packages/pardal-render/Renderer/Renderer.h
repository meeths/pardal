
#pragma once
#include <Memory/SharedPointer.h>
#include <Renderer/IRenderDevice.h>
#include <Renderer/Shaders/device_host_structs.h>
#include <Renderer/PipelineState.h>
#include <ImGui/ImGuiRenderer.h>

#include "Math/Rectangle.h"

// Created on 2025-03-23 by sisco

namespace pdl
{
    class BindlessDescriptors;
}

namespace pdl
{
    class RenderPass;
    class IInternalRenderer;

class Renderer
{
public:
    bool InitializeRenderDevice(const IRenderDevice::InitInfoBase& initInfo);
    ~Renderer();
    SharedPointer<IRenderDevice> GetRenderDevice() const { return m_device; }

    void Update(float deltaTime);
    bool BeginFrame();
    bool EndFrame();

    RenderPass& GetMainRenderPass();
    bool BeginRenderPass(const RenderPass& renderPass);
    bool EndRenderPass();

    PerFrameInfo& GetFrameInfo() { return m_frameInfo; }

    BindlessDescriptors* GetBindlessDescriptors() const { return m_bindlessDescriptors.get(); }

    void SetPipelineState(const PipelineState& pipelineState);
    void SetViewport(const Math::Rectangle& viewport);
    void SetScissor(const Math::Rectanglei& scissor);
    
private:

    void UpdateFrameInfoBuffer();
    
    SharedPointer<IRenderDevice> m_device;
    SharedPointer<IInternalRenderer> m_internalRenderer;
    SharedPointer<ImGuiRenderer> m_imguiRenderer;

    SharedPointer<BindlessDescriptors> m_bindlessDescriptors;

    PerFrameInfo m_frameInfo {};
    SharedPointer<IRenderBuffer> m_frameInfoBuffer;

    SharedPointer<IShader> m_testShader;
};

}
