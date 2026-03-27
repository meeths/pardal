
#include "Render/RenderTargetCache.h"

#include "Log/Log.h"

// Created on 2026-03-27 by Sisco

namespace pdl
{

RenderTargetCache::~RenderTargetCache()
{
    if (!m_rhi)
        return;

    for (auto& [name, entry] : m_entries)
    {
        if (entry.handle.IsValid())
            m_rhi->Destroy(entry.handle);
    }
}

void RenderTargetCache::Declare(StringView name, TextureDesc desc)
{
    if (name == Swapchain)
        return; // Swapchain is always resolved from the RHI, never allocated here.

    String key(name);
    if (m_entries.find(key) != m_entries.end())
        return; // First declaration wins.

    const bool isAutoSized = (desc.width == 0 || desc.height == 0);
    m_entries[key] = {desc, {}, isAutoSized};
}

void RenderTargetCache::Build(IRHIContext& rhi)
{
    m_rhi = &rhi;

    const TextureHandle swapTex  = rhi.GetCurrentSwapchainTexture();
    const auto          swapDims = rhi.GetDimensions(swapTex);

    for (auto& [name, entry] : m_entries)
    {
        CreateEntry(entry, rhi, swapDims.width, swapDims.height);
    }
}

void RenderTargetCache::Resize(IRHIContext& rhi, uint32 width, uint32 height)
{
    m_rhi = &rhi;

    for (auto& [name, entry] : m_entries)
    {
        if (!entry.isAutoSized)
            continue;

        if (entry.handle.IsValid())
        {
            rhi.Destroy(entry.handle);
            entry.handle = {};
        }

        CreateEntry(entry, rhi, width, height);
    }
}

TextureHandle RenderTargetCache::Resolve(StringView name, IRHIContext& rhi) const
{
    if (name == Swapchain)
        return rhi.GetCurrentSwapchainTexture();

    const auto it = m_entries.find(String(name));
    if (it == m_entries.end())
    {
        pdlLogError("RenderTargetCache: Unknown render target '%s'", name.data());
        return {};
    }

    return it->second.handle;
}

bool RenderTargetCache::IsKnown(StringView name) const
{
    if (name == Swapchain)
        return true;
    return m_entries.find(String(name)) != m_entries.end();
}

void RenderTargetCache::CreateEntry(Entry& entry, IRHIContext& rhi, uint32 width, uint32 height)
{
    if (entry.isAutoSized)
    {
        entry.desc.width  = width;
        entry.desc.height = height;
    }

    auto result = rhi.CreateTexture(entry.desc);
    if (!result.has_value())
    {
        pdlLogError("RenderTargetCache: Failed to create texture: %s", result.error().c_str());
        return;
    }

    entry.handle = *result;
}

} // namespace pdl
