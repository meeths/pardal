#include "Geometry/GeometryData.h"

// Created on 2026-03-20 by sisco

namespace pdl
{

VertexInput VertexBufferData::BuildVertexInput(uint32 binding) const
{
    VertexInput vi;

    vi.numBindings  = 1;
    vi.bindings[0]  = { .binding = binding, .stride = stride };

    const uint32 attrCount = static_cast<uint32>(attributes.size());
    vi.numAttributes = attrCount < VertexInput::MaxAttributes ? attrCount : VertexInput::MaxAttributes;

    for (uint32 i = 0; i < vi.numAttributes; ++i)
    {
        vi.attributes[i] =
        {
            .location = i,
            .binding  = binding,
            .format   = attributes[i].format,
            .offset   = attributes[i].offset,
        };
    }

    return vi;
}

} // namespace pdl
