
#pragma once
#include <Memory/SharedPointer.h>
#include <Renderer/IInternalRenderer.h>
#include <Renderer/PipelineState.h>
#include <Renderer/RendererTypes.h>
#include <Renderer/RenderPass.h>

// Created on 2025-03-26 by sisco

namespace pdl
{
    class VulkanTextureView;
    class VulkanTexture;
    class VulkanSurface;
}

namespace pdl
{
class VulkanDevice;

class VulkanInternalRenderer : public IInternalRenderer
{
public:
    VulkanInternalRenderer(VulkanDevice& device);
    ~VulkanInternalRenderer() override = default;

    bool Initialize(const InitInfo& initInfo) override;
    
    bool BeginFrame() override;
    bool EndFrame() override;

    RenderPass& GetMainRenderPass() override { return m_mainRenderPass; }

    bool BeginRenderPass(const RenderPass& renderPass) override;
    bool EndRenderPass() override;
    void OnResize(Math::Vector2i newSize) override;

    void SetPipelineState(const PipelineState& pipelineState, bool force) override;
private:
    bool BuildDepthBuffer(Format format, Math::Vector2i size);

public:

private:
    VulkanDevice& m_device;
    
    const RenderPass* m_lastRenderPass = nullptr;
    SharedPointer<VulkanSurface> m_surface;
    SharedPointer<VulkanTexture> m_depthTexture;
    SharedPointer<VulkanTextureView> m_depthTextureView;
    RenderPass m_mainRenderPass;

    bool m_pipelineStateInitialized = false;
    PipelineState m_currentPipelineState {};
};

}

