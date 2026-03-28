
#ifdef PDL_FEATURE_IMGUI

#include "Render/Passes/DebugRenderGraphPass.h"

#include "Base/ServiceLocator.h"
#include "ImGui/ImGuiRenderer.h"
#include "Render/Passes/EditorRenderGraphPass.h"
#include "Render/RenderTargetCache.h"

// Created on 2026-03-27 by Sisco

namespace pdl
{

void DebugRenderGraphPass::Setup(RenderGraphPassBuilder& builder)
{
    // Load existing colour and depth from the Editor pass — no clear.
    // The RenderGraph detects LoadOp::Load and schedules this pass after any
    // pass that writes these targets with a non-Load op (i.e. the Editor pass).
    builder.WriteColorTarget({
        .targetName = String(RenderTargetCache::Swapchain),
        .loadOp     = LoadOp::Load,
        .storeOp    = StoreOp::Store,
    });

    // Load the shared depth target so ImGui can depth-test if needed.
    // StoreOp::DontCare: nothing reads depth after this final pass.
    builder.WriteDepthTarget({
        .targetName = String(EditorRenderGraphPass::DepthTargetName),
        .loadOp     = LoadOp::Load,
        .storeOp    = StoreOp::DontCare,
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
