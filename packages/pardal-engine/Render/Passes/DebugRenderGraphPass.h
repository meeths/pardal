
#pragma once
#ifdef PDL_FEATURE_IMGUI

#include "Render/RenderGraphPass.h"

// Created on 2026-03-27 by Sisco

namespace pdl
{

// Renders the ImGui overlay as the final pass in the render graph.
//
// Uses LoadOp::Load on the swapchain so it composites on top of whatever the
// previous passes wrote.  The graph automatically orders this after any pass
// that produces the swapchain with a non-Load op.
class DebugRenderGraphPass : public RenderGraphPass
{
public:
    static constexpr StringView PassName = "Debug";

    StringView GetName() const override { return PassName; }

    // Declares: write to Swapchain with LoadOp::Load (overlay / no clear).
    void Setup(RenderGraphPassBuilder& builder) override;

    // Opens a rendering scope on the swapchain, draws ImGui, then closes it.
    void Execute(RenderGraphPassContext& ctx) override;
};

} // namespace pdl

#endif // PDL_FEATURE_IMGUI
