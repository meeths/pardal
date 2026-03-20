#pragma once
#include "Base/BaseTypes.h"
#include "Containers/Vector.h"
#include "Renderer/RHIDescriptors.h"

// Created on 2026-03-20 by sisco

namespace pdl
{

// High-level semantic tag for a vertex attribute.
// semanticIndex distinguishes multiple channels of the same kind (e.g. TexCoord0 vs TexCoord1).
enum class VertexAttributeSemantic : uint8
{
    Position,
    Normal,
    Tangent,
    Binormal,
    Color,
    TexCoord,
    BoneIndices,
    BoneWeights,
};

struct VertexAttributeDesc
{
    VertexAttributeSemantic semantic;
    Format                  format;
    uint32                  offset;        // Byte offset within a single vertex
    uint8                   semanticIndex = 0;
};

// Raw vertex buffer: tightly-packed byte blob with a descriptor list that
// explains what each field in the stride means.
struct VertexBufferData
{
    Vector<uint8>             data;
    uint32                    stride      = 0;
    uint32                    vertexCount = 0;
    Vector<VertexAttributeDesc> attributes;

    // Builds a VertexInput for use in RenderPipelineDesc.
    // Attributes are assigned shader locations in array order (0, 1, 2 ...).
    [[nodiscard]] VertexInput BuildVertexInput(uint32 binding = 0) const;
};

struct GeometryData
{
    VertexBufferData vertexBuffer;
    Vector<uint32>   indices;
};

} // namespace pdl
