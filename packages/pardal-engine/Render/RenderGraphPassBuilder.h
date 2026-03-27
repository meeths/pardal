
#pragma once
#include "Base/Optional.h"
#include "Containers/Vector.h"
#include "Math/Vector4.h"
#include "Renderer/RHIDescriptors.h"
#include "Renderer/RendererTypes.h"
#include "String/String.h"

// Created on 2026-03-27 by Sisco

namespace pdl
{

// Captures everything a RenderGraphPass declares during its Setup() phase.
// The RenderGraph reads this data to resolve render targets, build RenderPass /
// Framebuffer structs, and derive the execution order.
class RenderGraphPassBuilder
{
public:
    // A color attachment written by this pass.
    struct ColorAttachment
    {
        String        targetName;
        LoadOp        loadOp     = LoadOp::Clear;
        StoreOp       storeOp    = StoreOp::Store;
        Math::Vector4 clearColor = {0.0f, 0.0f, 0.0f, 1.0f};
    };

    // A depth/stencil attachment written by this pass.
    struct DepthAttachment
    {
        String  targetName;
        LoadOp  loadOp     = LoadOp::Clear;
        StoreOp storeOp    = StoreOp::DontCare;
        float   clearDepth = 1.0f;
    };

    // A new render target declared by this pass (allocates GPU memory on Build).
    // Set width = 0 / height = 0 to match the swapchain dimensions automatically.
    struct TargetDeclaration
    {
        String      name;
        TextureDesc desc;
    };

    // --- Declaration API (called from RenderGraphPass::Setup) ----------------

    void WriteColorTarget(ColorAttachment attachment)
    {
        m_colorAttachments.push_back(std::move(attachment));
    }

    void WriteDepthTarget(DepthAttachment attachment)
    {
        m_depthAttachment = std::move(attachment);
    }

    // Declares that this pass reads from `targetName` as a shader input.
    // The graph will ensure the writing pass executes before this one.
    void ReadTarget(StringView targetName)
    {
        m_readTargets.push_back(String(targetName));
    }

    // Registers a new named render target to be created during Build().
    void DeclareTarget(StringView name, TextureDesc desc)
    {
        m_declaredTargets.push_back({String(name), desc});
    }

    // Forces this pass to run after `passName`, even with no shared render targets.
    void DependsOn(StringView passName)
    {
        m_explicitDependencies.push_back(String(passName));
    }

    // --- Accessors (read by RenderGraph) -------------------------------------

    const Vector<ColorAttachment>&   GetColorAttachments()     const { return m_colorAttachments; }
    const Optional<DepthAttachment>& GetDepthAttachment()      const { return m_depthAttachment; }
    const Vector<String>&            GetReadTargets()          const { return m_readTargets; }
    const Vector<TargetDeclaration>& GetDeclaredTargets()      const { return m_declaredTargets; }
    const Vector<String>&            GetExplicitDependencies() const { return m_explicitDependencies; }

private:
    Vector<ColorAttachment>   m_colorAttachments;
    Optional<DepthAttachment> m_depthAttachment;
    Vector<String>            m_readTargets;
    Vector<TargetDeclaration> m_declaredTargets;
    Vector<String>            m_explicitDependencies;
};

} // namespace pdl
