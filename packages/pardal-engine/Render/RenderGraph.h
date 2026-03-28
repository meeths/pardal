
#pragma once
#include "Containers/Vector.h"
#include "Memory/UniquePointer.h"
#include "Render/RenderGraphPass.h"
#include "Render/RenderTargetCache.h"

// Created on 2026-03-27 by Sisco

namespace pdl
{
class IRHIContext;
class InputManager;

class RenderGraph
{
public:
    // Register a render pass. Passes may be added in any order.
    void AddPass(UniquePointer<RenderGraphPass> pass);

    // Calls Setup() on every pass, registers declared render targets, creates GPU
    // resources, then derives a topological execution order from:
    //   1. Shared render targets (LoadOp::Load implies a dependency on the producer).
    //   2. Explicit ReadTarget() declarations.
    //   3. Explicit DependsOn() constraints.
    void Build(IRHIContext& rhi);

    // Calls Update() on every pass in dependency-resolved order.
    // Must be called before Execute() each frame.
    void Update(float deltaTime, const InputManager& input);

    // Acquires a command buffer, executes passes in dependency-resolved order,
    // and submits the frame for presentation.
    void Execute(IRHIContext& rhi);

    // Recreates auto-sized render targets (width/height = 0 at declaration time)
    // to match the new window size. Call this from a window resize callback.
    void Resize(IRHIContext& rhi, uint32 width, uint32 height);

private:
    struct PassEntry
    {
        UniquePointer<RenderGraphPass> pass;
        RenderGraphPassBuilder         builder;
        RenderPass                     resolvedRenderPass;
    };

    static RenderPass  BuildRenderPass(const RenderGraphPassBuilder& builder);
    Framebuffer        BuildFramebuffer(const RenderGraphPassBuilder& builder, IRHIContext& rhi) const;

    Vector<PassEntry>  m_passes;
    Vector<uint32>     m_sortedIndices;
    RenderTargetCache  m_targetCache;
};

} // namespace pdl
