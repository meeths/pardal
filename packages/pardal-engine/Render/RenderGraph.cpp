
#include "Render/RenderGraph.h"

#include "Containers/UnorderedMap.h"
#include "Input/InputManager.h"
#include "Log/Log.h"
#include "Renderer/IRHIContext.h"
#include "Renderer/RenderererConstants.h"

// Created on 2026-03-27 by Sisco

namespace pdl
{

// ---------------------------------------------------------------------------
// Pass registration
// ---------------------------------------------------------------------------

void RenderGraph::AddPass(UniquePointer<RenderGraphPass> pass)
{
    m_passes.push_back({std::move(pass), {}, {}});
    m_sortedIndices.clear();
}

// ---------------------------------------------------------------------------
// Build — Setup, target creation, dependency resolution, topological sort
// ---------------------------------------------------------------------------

void RenderGraph::Build(IRHIContext& rhi)
{
    m_sortedIndices.clear();

    const uint32 passCount = static_cast<uint32>(m_passes.size());

    // Phase 1: Run Setup() on each pass to collect builder data.
    for (auto& entry : m_passes)
    {
        entry.builder = {};
        entry.pass->Setup(entry.builder);
    }

    // Phase 2: Register all declared targets with the cache, then allocate GPU resources.
    for (const auto& entry : m_passes)
    {
        for (const auto& decl : entry.builder.GetDeclaredTargets())
        {
            m_targetCache.Declare(decl.name, decl.desc);
        }
    }
    m_targetCache.Build(rhi);

    // Phase 3: Pre-build each pass's stable pdl::RenderPass (load/store ops, clear values).
    for (auto& entry : m_passes)
    {
        entry.resolvedRenderPass = BuildRenderPass(entry.builder);
    }

    // Phase 4: Derive execution order.

    // Map pass name → index.
    UnorderedMap<String, uint32> nameToIndex;
    nameToIndex.reserve(passCount);
    for (uint32 i = 0; i < passCount; ++i)
        nameToIndex[String(m_passes[i].pass->GetName())] = i;

    // For every target, record which passes write to it and with what LoadOp.
    struct WriterInfo { uint32 passIndex; LoadOp loadOp; };
    UnorderedMap<String, Vector<WriterInfo>> writtenBy;

    for (uint32 i = 0; i < passCount; ++i)
    {
        const auto& builder = m_passes[i].builder;
        for (const auto& ca : builder.GetColorAttachments())
            writtenBy[ca.targetName].push_back({i, ca.loadOp});
        if (const auto& da = builder.GetDepthAttachment())
            writtenBy[da->targetName].push_back({i, da->loadOp});
    }

    // deps[i] holds the indices of passes that must complete before pass i.
    Vector<Vector<uint32>> deps;
    deps.resize(passCount);

    // Implicit rule: a pass writing target T with LoadOp::Load depends on every
    // other pass that writes T with a non-Load op (i.e. the producers of T).
    for (auto& [target, writers] : writtenBy)
    {
        for (uint32 b = 0; b < static_cast<uint32>(writers.size()); ++b)
        {
            if (writers[b].loadOp != LoadOp::Load)
                continue;

            for (uint32 a = 0; a < static_cast<uint32>(writers.size()); ++a)
            {
                if (a == b || writers[a].loadOp == LoadOp::Load)
                    continue;

                deps[writers[b].passIndex].push_back(writers[a].passIndex);
            }
        }
    }

    // Explicit read dependencies: pass i reads target → depends on all writers of that target.
    for (uint32 i = 0; i < passCount; ++i)
    {
        for (const auto& readTarget : m_passes[i].builder.GetReadTargets())
        {
            auto it = writtenBy.find(readTarget);
            if (it == writtenBy.end())
                continue;
            for (const auto& w : it->second)
            {
                if (w.passIndex != i)
                    deps[i].push_back(w.passIndex);
            }
        }
    }

    // Explicit name-based dependencies.
    for (uint32 i = 0; i < passCount; ++i)
    {
        for (const auto& depName : m_passes[i].builder.GetExplicitDependencies())
        {
            auto it = nameToIndex.find(depName);
            if (it == nameToIndex.end())
            {
                pdlLogError("RenderGraph: Pass '%s' has unknown explicit dependency '%s'",
                            m_passes[i].pass->GetName().data(), depName.c_str());
                continue;
            }
            if (it->second != i)
                deps[i].push_back(it->second);
        }
    }

    // Kahn's algorithm for topological sort.
    // inDegree[i] counts unresolved dependencies for pass i.
    // dependents[i] lists passes that depend on pass i.
    Vector<uint32> inDegree;
    inDegree.resize(passCount, 0u);
    Vector<Vector<uint32>> dependents;
    dependents.resize(passCount);

    for (uint32 i = 0; i < passCount; ++i)
    {
        for (const uint32 dep : deps[i])
        {
            ++inDegree[i];
            dependents[dep].push_back(i);
        }
    }

    Vector<uint32> ready;
    for (uint32 i = 0; i < passCount; ++i)
    {
        if (inDegree[i] == 0)
            ready.push_back(i);
    }

    m_sortedIndices.reserve(passCount);
    while (!ready.empty())
    {
        const uint32 idx = ready.back();
        ready.pop_back();
        m_sortedIndices.push_back(idx);

        for (const uint32 dependent : dependents[idx])
        {
            if (--inDegree[dependent] == 0)
                ready.push_back(dependent);
        }
    }

    if (m_sortedIndices.size() != static_cast<size_t>(passCount))
    {
        pdlLogError("RenderGraph: Dependency cycle detected — %zu of %u passes will execute",
                    m_sortedIndices.size(), passCount);
    }
}

// ---------------------------------------------------------------------------
// Update — per-frame logic (input, camera, etc.)
// ---------------------------------------------------------------------------

void RenderGraph::Update(float deltaTime, const InputManager& input)
{
    for (const uint32 idx : m_sortedIndices)
        m_passes[idx].pass->Update(deltaTime, input);
}

// ---------------------------------------------------------------------------
// Execute — one frame
// ---------------------------------------------------------------------------

void RenderGraph::Execute(IRHIContext& rhi)
{
    if (!m_passes.empty() && m_sortedIndices.empty())
    {
        pdlLogError("RenderGraph::Execute called before Build()");
        return;
    }

    const TextureHandle presentTarget = rhi.GetCurrentSwapchainTexture();
    IRHICommandBuffer&  cmd           = rhi.AcquireCommandBuffer();

    for (const uint32 idx : m_sortedIndices)
    {
        auto&      entry      = m_passes[idx];
        Framebuffer framebuffer = BuildFramebuffer(entry.builder, rhi);

        RenderGraphPassContext ctx{cmd, rhi, entry.resolvedRenderPass, framebuffer};
        entry.pass->Execute(ctx);
    }

    rhi.Submit(cmd, presentTarget);
}

// ---------------------------------------------------------------------------
// Resize — recreate auto-sized targets
// ---------------------------------------------------------------------------

void RenderGraph::Resize(IRHIContext& rhi, uint32 width, uint32 height)
{
    m_targetCache.Resize(rhi, width, height);
}

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

RenderPass RenderGraph::BuildRenderPass(const RenderGraphPassBuilder& builder)
{
    RenderPass rp;

    uint32 colorIdx = 0;
    for (const auto& ca : builder.GetColorAttachments())
    {
        if (colorIdx >= RenderererConstants::MaxColorAttachments())
            break;
        auto& att      = rp.m_colorAttachments[colorIdx];
        att.clearColor = ca.clearColor;
        att.loadOp     = ca.loadOp;
        att.storeOp    = ca.storeOp;
        ++colorIdx;
    }

    if (const auto& da = builder.GetDepthAttachment())
    {
        rp.m_depthAttachment.clearDepth = da->clearDepth;
        rp.m_depthAttachment.loadOp     = da->loadOp;
        rp.m_depthAttachment.storeOp    = da->storeOp;
    }

    return rp;
}

Framebuffer RenderGraph::BuildFramebuffer(const RenderGraphPassBuilder& builder,
                                          IRHIContext&                   rhi) const
{
    Framebuffer fb;

    uint32 colorIdx = 0;
    for (const auto& ca : builder.GetColorAttachments())
    {
        if (colorIdx >= RenderererConstants::MaxColorAttachments())
            break;
        fb.m_colorAttachments[colorIdx].m_texture = m_targetCache.Resolve(ca.targetName, rhi);
        ++colorIdx;
    }

    if (const auto& da = builder.GetDepthAttachment())
    {
        fb.m_depthStencilTexture.m_texture = m_targetCache.Resolve(da->targetName, rhi);
    }

    return fb;
}

} // namespace pdl
