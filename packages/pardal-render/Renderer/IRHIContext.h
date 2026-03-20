
#pragma once
#include "Renderer/IRHICommandBuffer.h"
#include "Renderer/RHIDescriptors.h"
#include "Renderer/RenderererInfo.h"
#include "Base/BaseTypes.h"
#include "Base/Expected.h"
#include "String/String.h"
#include <utility>

// Created on 2026-03-20 by sisco

namespace pdl
{

class IRHIContext
{
public:
    struct Dimensions
    {
        uint32 width  = 1;
        uint32 height = 1;
        uint32 depth  = 1;
    };

    virtual ~IRHIContext() = default;

    // Resource creation
    [[nodiscard]] virtual Expected<TextureHandle, String>         CreateTexture(const TextureDesc& desc) = 0;
    [[nodiscard]] virtual Expected<BufferHandle, String>          CreateBuffer(const BufferDesc& desc) = 0;
    [[nodiscard]] virtual Expected<SamplerHandle, String>         CreateSampler(const SamplerDesc& desc) = 0;
    [[nodiscard]] virtual Expected<ShaderModuleHandle, String>    CreateShaderModule(const ShaderModuleDesc& desc) = 0;
    [[nodiscard]] virtual Expected<RenderPipelineHandle, String>  CreateRenderPipeline(const RenderPipelineDesc& desc) = 0;
    [[nodiscard]] virtual Expected<ComputePipelineHandle, String> CreateComputePipeline(const ComputePipelineDesc& desc) = 0;

    // Resource destruction
    virtual void Destroy(TextureHandle handle) = 0;
    virtual void Destroy(BufferHandle handle) = 0;
    virtual void Destroy(SamplerHandle handle) = 0;
    virtual void Destroy(ShaderModuleHandle handle) = 0;
    virtual void Destroy(RenderPipelineHandle handle) = 0;
    virtual void Destroy(ComputePipelineHandle handle) = 0;

    // Buffer data
    [[nodiscard]] virtual void*          GetMappedPtr(BufferHandle handle) = 0;
    virtual Expected<void, String>       Upload(BufferHandle handle, const void* data, size_t size, uint64 offset = 0) = 0;
    virtual Expected<void, String>       Download(BufferHandle handle, void* data, size_t size, uint64 offset = 0) = 0;

    // Texture info
    [[nodiscard]] virtual Dimensions     GetDimensions(TextureHandle handle) const = 0;
    [[nodiscard]] virtual Format         GetTextureFormat(TextureHandle handle) const = 0;

    // Swapchain
    [[nodiscard]] virtual TextureHandle  GetCurrentSwapchainTexture() = 0;
    [[nodiscard]] virtual Format         GetSwapchainFormat() const = 0;

    // Frame
    virtual IRHICommandBuffer&           AcquireCommandBuffer() = 0;
    virtual void                         Submit(IRHICommandBuffer& cmdBuf, TextureHandle presentTexture = {}) = 0;

    // Sync
    virtual void WaitIdle() = 0;

    // Info
    [[nodiscard]] virtual const RenderDeviceInfo& GetDeviceInfo() const = 0;
};

// RAII holder: auto-destroys the handle when it goes out of scope.
// Mirrors lvk::Holder but operates on IRHIContext.
template<typename HandleType>
class Holder final
{
public:
    Holder() = default;

    Holder(IRHIContext* ctx, HandleType handle) : m_ctx(ctx), m_handle(handle) {}

    ~Holder()
    {
        Reset();
    }

    Holder(const Holder&) = delete;
    Holder& operator=(const Holder&) = delete;

    Holder(Holder&& other) noexcept : m_ctx(other.m_ctx), m_handle(other.m_handle)
    {
        other.m_ctx    = nullptr;
        other.m_handle = HandleType{};
    }

    Holder& operator=(Holder&& other) noexcept
    {
        std::swap(m_ctx, other.m_ctx);
        std::swap(m_handle, other.m_handle);
        return *this;
    }

    operator HandleType() const { return m_handle; }
    HandleType Get() const      { return m_handle; }

    bool IsValid() const { return m_handle.IsValid(); }
    bool IsEmpty() const { return m_handle.IsEmpty(); }

    void Reset()
    {
        if (m_ctx && m_handle.IsValid())
        {
            m_ctx->Destroy(m_handle);
        }
        m_ctx    = nullptr;
        m_handle = HandleType{};
    }

    HandleType Release()
    {
        m_ctx          = nullptr;
        HandleType h   = m_handle;
        m_handle       = HandleType{};
        return h;
    }

private:
    IRHIContext* m_ctx    = nullptr;
    HandleType   m_handle = {};
};

} // namespace pdl
