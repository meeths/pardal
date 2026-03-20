
#pragma once
#include "Renderer/RHIDescriptors.h"
#include "Renderer/RendererTypes.h"
#include "Math/Vector4.h"
#include "Base/BaseTypes.h"

// Created on 2026-03-20 by sisco

namespace pdl
{

class IRHICommandBuffer
{
public:
    virtual ~IRHICommandBuffer() = default;

    virtual void CmdBeginRendering(const RenderPass& renderPass, const Framebuffer& framebuffer) = 0;
    virtual void CmdEndRendering() = 0;

    virtual void CmdBindRenderPipeline(RenderPipelineHandle handle) = 0;
    virtual void CmdBindComputePipeline(ComputePipelineHandle handle) = 0;

    virtual void CmdBindIndexBuffer(BufferHandle buffer, IndexFormat format, uint64 offset = 0) = 0;
    virtual void CmdBindVertexBuffer(uint32 index, BufferHandle buffer, uint64 offset = 0) = 0;

    virtual void CmdPushConstants(const void* data, size_t size, uint32 offset = 0) = 0;

    template<typename T>
    void CmdPushConstants(const T& data, uint32 offset = 0)
    {
        CmdPushConstants(&data, sizeof(T), offset);
    }

    virtual void CmdDraw(uint32 vertexCount, uint32 instanceCount = 1, uint32 firstVertex = 0, uint32 firstInstance = 0) = 0;
    virtual void CmdDrawIndexed(uint32 indexCount, uint32 instanceCount = 1, uint32 firstIndex = 0, int32 vertexOffset = 0, uint32 firstInstance = 0) = 0;
    virtual void CmdDrawIndirect(BufferHandle buffer, uint64 offset, uint32 drawCount, uint32 stride = 0) = 0;
    virtual void CmdDrawIndexedIndirect(BufferHandle buffer, uint64 offset, uint32 drawCount, uint32 stride = 0) = 0;

    virtual void CmdDispatch(uint32 x, uint32 y, uint32 z = 1) = 0;

    virtual void CmdSetViewport(const Viewport& viewport) = 0;
    virtual void CmdSetScissorRect(const ScissorRect& rect) = 0;
    virtual void CmdSetDepthBias(const DepthBias& bias) = 0;

    virtual void CmdTransitionImageLayout(TextureHandle texture, ResourceState src, ResourceState dst, const SubresourceRange& range = {}) = 0;
    virtual void CmdCopyImage(TextureHandle src, ResourceState srcState, TextureHandle dst, ResourceState dstState) = 0;
    virtual void CmdClearColorImage(TextureHandle texture, const Math::Vector4& clearColor, ResourceState state) = 0;
    virtual void CmdGenerateMipmap(TextureHandle texture) = 0;

    virtual void CmdCopyBuffer(BufferHandle src, uint64 srcOffset, BufferHandle dst, uint64 dstOffset, uint64 size) = 0;
    virtual void CmdUpdateBuffer(BufferHandle buffer, uint64 offset, const void* data, uint64 size) = 0;

    virtual void CmdPushDebugGroupLabel(const char* label) = 0;
    virtual void CmdPopDebugGroupLabel() = 0;
    virtual void CmdInsertDebugEventLabel(const char* label) = 0;
};

} // namespace pdl
