
#pragma once
#include <Math/Vector2i.h>

// Created on 2025-03-26 by sisco


namespace pdl
{
namespace Math
{
    class Rectangle;
    class Rectanglei;
}
class ApplicationWindow;
class RenderPass;
class ITextureView;
struct PipelineState;
class ICommandBuffer;
    
class IInternalRenderer
{
public:
    struct InitInfo
    {
        ApplicationWindow& m_window;
        Math::Vector2i m_initialSurfaceSize;
        bool m_useVSync = false;
        bool m_useHDR = false;
        bool m_createDepthBuffer = true;
    };
    
    virtual ~IInternalRenderer() = default;

    virtual bool Initialize(const InitInfo& initInfo) = 0;
    
    virtual bool BeginFrame() = 0;
    virtual bool EndFrame() = 0;

    virtual RenderPass& GetMainRenderPass() = 0;
    
    virtual void OnResize(Math::Vector2i newSize) = 0;
    
    virtual bool BeginRenderPass(const RenderPass& renderPass) = 0;
    virtual bool EndRenderPass() = 0;

    virtual void SetPipelineState(ICommandBuffer* cmd, const PipelineState& pipelineState, bool force = false) = 0;

    virtual void SetViewport(ICommandBuffer* cmd, const Math::Rectangle& viewport) = 0;
    virtual void SetScissor(ICommandBuffer* cmd, const Math::Rectanglei& scissor) = 0;
    
    virtual ICommandBuffer* GetCommandBuffer() = 0;
};

}

