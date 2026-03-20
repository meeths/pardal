
#pragma once
#include "Base/BaseDefines.h"
#include "Containers/Pool.h"
#include "Memory/UniquePointer.h"
#include "Renderer/IRHIContext.h"
#include "Renderer/Vulkan/lvk/LVK.h"
#include "Renderer/Vulkan/lvk/vulkan/VulkanClasses.h"

// Created on 2026-03-20 by sisco

namespace pdl
{

// Vulkan (LVK-backed) implementation of IRHIContext.
// RHI handles are pool indices into VulkanRHIContext's own resource tables,
// which store the corresponding lvk handles. No bit_cast between layers.
class VulkanRHIContext final : public IRHIContext
{
public:
    explicit VulkanRHIContext(lvk::IContext* lvkCtx);
    ~VulkanRHIContext() override;

    DeclareNonCopyable(VulkanRHIContext);

    // Factory for headless (windowless) contexts — primarily used in tests
    [[nodiscard]] static UniquePointer<VulkanRHIContext> CreateHeadless(bool enableValidation = false);

    // IRHIContext
    [[nodiscard]] Expected<TextureHandle, String>         CreateTexture(const TextureDesc& desc) override;
    [[nodiscard]] Expected<BufferHandle, String>          CreateBuffer(const BufferDesc& desc) override;
    [[nodiscard]] Expected<SamplerHandle, String>         CreateSampler(const SamplerDesc& desc) override;
    [[nodiscard]] Expected<ShaderModuleHandle, String>    CreateShaderModule(const ShaderModuleDesc& desc) override;
    [[nodiscard]] Expected<RenderPipelineHandle, String>  CreateRenderPipeline(const RenderPipelineDesc& desc) override;
    [[nodiscard]] Expected<ComputePipelineHandle, String> CreateComputePipeline(const ComputePipelineDesc& desc) override;

    void Destroy(TextureHandle handle) override;
    void Destroy(BufferHandle handle) override;
    void Destroy(SamplerHandle handle) override;
    void Destroy(ShaderModuleHandle handle) override;
    void Destroy(RenderPipelineHandle handle) override;
    void Destroy(ComputePipelineHandle handle) override;

    [[nodiscard]] void*          GetMappedPtr(BufferHandle handle) override;
    Expected<void, String>       Upload(BufferHandle handle, const void* data, size_t size, uint64 offset = 0) override;
    Expected<void, String>       Download(BufferHandle handle, void* data, size_t size, uint64 offset = 0) override;

    [[nodiscard]] Dimensions     GetDimensions(TextureHandle handle) const override;
    [[nodiscard]] Format         GetTextureFormat(TextureHandle handle) const override;

    [[nodiscard]] TextureHandle  GetCurrentSwapchainTexture() override;
    [[nodiscard]] Format         GetSwapchainFormat() const override;

    IRHICommandBuffer&           AcquireCommandBuffer() override;
    void                         Submit(IRHICommandBuffer& cmdBuf, TextureHandle presentTexture = {}) override;

    void WaitIdle() override;

    [[nodiscard]] const RenderDeviceInfo& GetDeviceInfo() const override;

    // Escape hatches for Vulkan-specific integrations (e.g. ImGui backend)
    lvk::IContext*        GetLVKContext() const { return m_lvkCtx; }
    lvk::TextureHandle    GetLVKTexture(TextureHandle h) const;
    static lvk::ICommandBuffer& GetLVKCommandBuffer(IRHICommandBuffer& cmd);

private:
    // Payload stored in the render-pipeline pool — includes the depth state
    // that LVK applies dynamically rather than baking into the pipeline object.
    struct RenderPipelineEntry
    {
        lvk::RenderPipelineHandle lvkHandle;
        DepthTest                 depthTest;
    };

    // Resource pools: RHI handle → lvk handle
    Pool<struct Texture,         lvk::TextureHandle>         m_textures;
    Pool<struct Buffer,          lvk::BufferHandle>          m_buffers;
    Pool<struct Sampler,         lvk::SamplerHandle>         m_samplers;
    Pool<struct ShaderModule,    lvk::ShaderModuleHandle>    m_shaderModules;
    Pool<struct RenderPipeline,  RenderPipelineEntry>        m_renderPipelines;
    Pool<struct ComputePipeline, lvk::ComputePipelineHandle> m_computePipelines;

    // Single reserved slot for the swapchain texture — updated each frame.
    TextureHandle m_swapchainHandle;

    // Lookup helpers
    lvk::TextureHandle           ToLvk(TextureHandle h) const;
    lvk::BufferHandle            ToLvk(BufferHandle h) const;
    lvk::SamplerHandle           ToLvk(SamplerHandle h) const;
    lvk::ShaderModuleHandle      ToLvk(ShaderModuleHandle h) const;
    lvk::RenderPipelineHandle    ToLvk(RenderPipelineHandle h) const;
    lvk::ComputePipelineHandle   ToLvk(ComputePipelineHandle h) const;

    lvk::Framebuffer ToFramebufferLvk(const Framebuffer& fb) const;

    // Command buffer wrapper — reused each frame
    class VulkanRHICommandBuffer final : public IRHICommandBuffer
    {
    public:
        lvk::ICommandBuffer* m_lvkCmdBuf = nullptr;
        VulkanRHIContext*    m_owner     = nullptr;

        void CmdBeginRendering(const RenderPass& renderPass, const Framebuffer& framebuffer) override;
        void CmdEndRendering() override;

        void CmdBindRenderPipeline(RenderPipelineHandle handle) override;
        void CmdBindComputePipeline(ComputePipelineHandle handle) override;

        void CmdBindIndexBuffer(BufferHandle buffer, IndexFormat format, uint64 offset) override;
        void CmdBindVertexBuffer(uint32 index, BufferHandle buffer, uint64 offset) override;

        void CmdPushConstants(const void* data, size_t size, uint32 offset) override;

        void CmdDraw(uint32 vertexCount, uint32 instanceCount, uint32 firstVertex, uint32 firstInstance) override;
        void CmdDrawIndexed(uint32 indexCount, uint32 instanceCount, uint32 firstIndex, int32 vertexOffset, uint32 firstInstance) override;
        void CmdDrawIndirect(BufferHandle buffer, uint64 offset, uint32 drawCount, uint32 stride) override;
        void CmdDrawIndexedIndirect(BufferHandle buffer, uint64 offset, uint32 drawCount, uint32 stride) override;

        void CmdDispatch(uint32 x, uint32 y, uint32 z) override;

        void CmdSetViewport(const Viewport& viewport) override;
        void CmdSetScissorRect(const ScissorRect& rect) override;
        void CmdSetDepthBias(const DepthBias& bias) override;

        void CmdTransitionImageLayout(TextureHandle texture, ResourceState src, ResourceState dst, const SubresourceRange& range) override;
        void CmdCopyImage(TextureHandle src, ResourceState srcState, TextureHandle dst, ResourceState dstState) override;
        void CmdClearColorImage(TextureHandle texture, const Math::Vector4& clearColor, ResourceState state) override;
        void CmdGenerateMipmap(TextureHandle texture) override;

        void CmdCopyBuffer(BufferHandle src, uint64 srcOffset, BufferHandle dst, uint64 dstOffset, uint64 size) override;
        void CmdUpdateBuffer(BufferHandle buffer, uint64 offset, const void* data, uint64 size) override;

        void CmdPushDebugGroupLabel(const char* label) override;
        void CmdPopDebugGroupLabel() override;
        void CmdInsertDebugEventLabel(const char* label) override;
    };

    [[maybe_unused]] UniquePointer<lvk::VulkanContext> m_ownedLvkCtx; // non-null only for headless contexts
    lvk::IContext*          m_lvkCtx = nullptr;
    VulkanRHICommandBuffer  m_commandBuffer;
    RenderDeviceInfo        m_deviceInfo;
};

} // namespace pdl
