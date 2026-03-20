#pragma once
#include "Base/BaseDefines.h"
#include "Base/BaseTypes.h"
#include "Base/Expected.h"
#include "String/String.h"
#include "Geometry/GeometryData.h"
#include "Renderer/IRHIContext.h"
#include "Renderer/IRHICommandBuffer.h"

// Created on 2026-03-20 by sisco

namespace pdl
{

// GPU mesh: owns device-local vertex and index buffers uploaded from a GeometryData.
// Move-only — use Mesh::Create() to construct.
class Mesh
{
public:
    Mesh() = default;

    DeclareNonCopyable(Mesh);
    DeclareDefaultMoveable(Mesh);

    // Uploads geometry to device-local buffers. Returns an error string on failure.
    [[nodiscard]] static Expected<Mesh, String> Create(IRHIContext& ctx, const GeometryData& data);

    // Binds the vertex buffer (and index buffer if present) to the command buffer.
    void Bind(IRHICommandBuffer& cmd) const;

    // Issues a non-indexed draw call. Call Bind() first.
    void Draw(IRHICommandBuffer& cmd, uint32 instanceCount = 1) const;

    // Issues an indexed draw call. Call Bind() first. Requires HasIndices() == true.
    void DrawIndexed(IRHICommandBuffer& cmd, uint32 instanceCount = 1) const;

    uint32 GetVertexCount() const { return m_vertexCount; }
    uint32 GetIndexCount()  const { return m_indexCount; }
    bool   HasIndices()     const { return m_indexBuffer.IsValid(); }

private:
    Holder<BufferHandle> m_vertexBuffer;
    Holder<BufferHandle> m_indexBuffer;
    uint32 m_vertexCount = 0;
    uint32 m_indexCount  = 0;
};

} // namespace pdl
