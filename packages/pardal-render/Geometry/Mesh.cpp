#include "Geometry/Mesh.h"

// Created on 2026-03-20 by sisco

namespace pdl
{

Expected<Mesh, String> Mesh::Create(IRHIContext& ctx, const GeometryData& data)
{
    if (data.vertexBuffer.data.empty())
        return Unexpected<String>("GeometryData has no vertex data");

    Mesh mesh;
    mesh.m_vertexCount = data.vertexBuffer.vertexCount;

    {
        auto result = ctx.CreateBuffer({
            .size        = data.vertexBuffer.data.size(),
            .usage       = BufferUsage::VertexBuffer,
            .storage     = MemoryType::DeviceLocal,
            .initialData = data.vertexBuffer.data.data(),
            .debugName   = "Mesh::VertexBuffer",
        });
        if (!result)
            return Unexpected<String>(result.error());
        mesh.m_vertexBuffer = Holder<BufferHandle>(&ctx, *result);
    }

    if (!data.indices.empty())
    {
        mesh.m_indexCount = static_cast<uint32>(data.indices.size());
        auto result = ctx.CreateBuffer({
            .size        = data.indices.size() * sizeof(uint32),
            .usage       = BufferUsage::IndexBuffer,
            .storage     = MemoryType::DeviceLocal,
            .initialData = data.indices.data(),
            .debugName   = "Mesh::IndexBuffer",
        });
        if (!result)
            return Unexpected<String>(result.error());
        mesh.m_indexBuffer = Holder<BufferHandle>(&ctx, *result);
    }

    return mesh;
}

void Mesh::Bind(IRHICommandBuffer& cmd) const
{
    cmd.CmdBindVertexBuffer(0, m_vertexBuffer.Get());
    if (m_indexBuffer.IsValid())
        cmd.CmdBindIndexBuffer(m_indexBuffer.Get(), IndexFormat::UInt32);
}

void Mesh::Draw(IRHICommandBuffer& cmd, uint32 instanceCount) const
{
    cmd.CmdDraw(m_vertexCount, instanceCount);
}

void Mesh::DrawIndexed(IRHICommandBuffer& cmd, uint32 instanceCount) const
{
    cmd.CmdDrawIndexed(m_indexCount, instanceCount);
}

} // namespace pdl
