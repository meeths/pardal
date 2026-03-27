
#pragma once
#include "Render/RenderGraphPassBuilder.h"
#include "Renderer/IRHICommandBuffer.h"
#include "Renderer/RendererTypes.h"

// Created on 2026-03-27 by Sisco

namespace pdl
{
class IRHIContext;

// Context provided to every pass during Execute().
// The RenderGraph resolves the framebuffer and render pass configuration before
// calling Execute, so passes can immediately call CmdBeginRendering.
struct RenderGraphPassContext
{
    IRHICommandBuffer&    cmd;
    IRHIContext&          rhi;
    const RenderPass&     renderPass;   // Pre-built from the pass's Setup declarations
    const Framebuffer&    framebuffer;  // Resolved render targets for this frame
};

class RenderGraphPass
{
public:
    virtual ~RenderGraphPass() = default;

    // Unique name used to identify this pass in the graph.
    virtual StringView GetName() const = 0;

    // Called once by RenderGraph::Build().
    // Declare render targets, attachments, and ordering constraints via builder.
    virtual void Setup(RenderGraphPassBuilder& builder) = 0;

    // Called every frame in dependency-resolved order.
    // The pass is responsible for CmdBeginRendering / CmdEndRendering.
    virtual void Execute(RenderGraphPassContext& ctx) = 0;
};

} // namespace pdl
