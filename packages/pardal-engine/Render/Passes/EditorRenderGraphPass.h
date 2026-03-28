
#pragma once
#ifdef PDL_FEATURE_IMGUI

#include "Geometry/Mesh.h"
#include "Render/RenderGraphPass.h"
#include "Renderer/IRHIContext.h"

// Created on 2026-03-28 by Sisco

namespace pdl
{

// Editor overlay pass — runs before the Debug (ImGui) pass.
//
// Clears the swapchain and the shared SceneDepth target, then renders
// editor-specific geometry (currently: a wireframe test cube).
// The Debug pass loads both targets with LoadOp::Load so the graph
// automatically schedules it after this pass.
class EditorRenderGraphPass : public RenderGraphPass
{
public:
    static constexpr StringView PassName       = "Editor";
    static constexpr StringView DepthTargetName = "SceneDepth";

    StringView GetName() const override { return PassName; }

    // Declares: clear swapchain + SceneDepth, declare the SceneDepth target.
    void Setup(RenderGraphPassBuilder& builder) override;

    // Lazy-initialises the wireframe pipeline and cube mesh on first call,
    // then records the draw commands.
    void Execute(RenderGraphPassContext& ctx) override;

private:
    void InitializeResources(IRHIContext& rhi);

    bool m_initialized = false;

    Holder<ShaderModuleHandle>   m_vertexShader;
    Holder<ShaderModuleHandle>   m_fragmentShader;
    Holder<RenderPipelineHandle> m_pipeline;
    Mesh                         m_cubeMesh;
};

} // namespace pdl

#endif // PDL_FEATURE_IMGUI
