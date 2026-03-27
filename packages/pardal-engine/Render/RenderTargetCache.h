
#pragma once
#include "Containers/UnorderedMap.h"
#include "Renderer/IRHIContext.h"
#include "Renderer/RHIDescriptors.h"
#include "Renderer/RendererTypes.h"
#include "String/String.h"

// Created on 2026-03-27 by Sisco

namespace pdl
{

// Manages named, GPU-allocated render targets for the RenderGraph.
//
// The special name "Swapchain" always resolves to the current swapchain image
// by querying the RHI each frame — it is never allocated by the cache itself.
//
// All other targets are created on Build() and owned by the cache until it is
// destroyed or Resize() recreates them.  Setting width = 0 / height = 0 in the
// TextureDesc marks a target as "swapchain-sized": it is automatically resized
// whenever Resize() is called.
class RenderTargetCache
{
public:
    static constexpr StringView Swapchain = "Swapchain";

    ~RenderTargetCache();

    // Registers a named target for later creation. Ignored for "Swapchain".
    // If the same name is declared more than once the first declaration wins.
    // width = 0 / height = 0  →  sized to match the swapchain (auto-sized).
    void Declare(StringView name, TextureDesc desc);

    // Creates GPU resources for all declared targets. Must be called before Resolve().
    void Build(IRHIContext& rhi);

    // Recreates every auto-sized target at the new resolution.
    void Resize(IRHIContext& rhi, uint32 width, uint32 height);

    // Returns the TextureHandle for the given name.
    // For "Swapchain" this calls rhi.GetCurrentSwapchainTexture() every time.
    TextureHandle Resolve(StringView name, IRHIContext& rhi) const;

    bool IsKnown(StringView name) const;

private:
    struct Entry
    {
        TextureDesc   desc;
        TextureHandle handle;
        bool          isAutoSized = false;
    };

    void CreateEntry(Entry& entry, IRHIContext& rhi, uint32 width, uint32 height);

    IRHIContext*                m_rhi = nullptr;
    UnorderedMap<String, Entry> m_entries;
};

} // namespace pdl
