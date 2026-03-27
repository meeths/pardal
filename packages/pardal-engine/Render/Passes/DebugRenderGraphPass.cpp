
#ifdef PDL_FEATURE_IMGUI

#include "Render/Passes/DebugRenderGraphPass.h"

#include "Base/ServiceLocator.h"
#include "ImGui/ImGuiRenderer.h"
#include "Render/RenderTargetCache.h"

// Created on 2026-03-27 by Sisco

namespace pdl
{

void DebugRenderGraphPass::Setup(RenderGraphPassBuilder& builder)
{
    // Write to the swapchain as an overlay: load existing content, then store.
    // The RenderGraph detects this LoadOp::Load and schedules this pass after any
    // pass that writes to the swapchain with a clearing op.
    builder.WriteColorTarget({
        .targetName = String(RenderTargetCache::Swapchain),
        .loadOp     = LoadOp::Clear,
        .storeOp    = StoreOp::Store,
        .clearColor = {0.08f, 0.08f, 0.10f, 1.0f},
    });
}

void DebugRenderGraphPass::Execute(RenderGraphPassContext& ctx)
{
#ifdef PDL_VULKAN
    const TextureHandle colorTarget = ctx.framebuffer.m_colorAttachments[0].m_texture;
    const TextureHandle depthTarget = ctx.framebuffer.m_depthStencilTexture.m_texture;

    auto& imGui = ServiceLocator<ImGuiRenderer>::Ref();

    // Open a Vulkan dynamic rendering scope for this pass.
    ctx.cmd.CmdBeginRendering(ctx.renderPass, ctx.framebuffer);

    // Set up the ImGui framebuffer target, then collect and record draw data.
    imGui.BeginFrame(colorTarget, depthTarget);
    imGui.Render();
    imGui.EndFrame(ctx.cmd);

    ctx.cmd.CmdEndRendering();
#endif
}

} // namespace pdl

#endif // PDL_FEATURE_IMGUI
