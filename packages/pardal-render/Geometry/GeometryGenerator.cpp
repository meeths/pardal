#include "Geometry/GeometryGenerator.h"
#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include <cmath>
#include <cstring>
#include <numbers>

// Created on 2026-03-20 by sisco

namespace pdl
{

namespace
{

// Local vertex type shared by all generators: position + normal + texcoord.
struct GenVertex
{
    Math::Vector3 position;
    Math::Vector3 normal;
    Math::Vector2 texCoord;
};

// Packs a vector of typed vertices into a VertexBufferData with attribute descriptors.
template<typename TVertex>
VertexBufferData PackVertices(Vector<TVertex>&& vertices, Vector<VertexAttributeDesc>&& attributes)
{
    VertexBufferData vbd;
    vbd.stride      = static_cast<uint32>(sizeof(TVertex));
    vbd.vertexCount = static_cast<uint32>(vertices.size());
    vbd.attributes  = std::move(attributes);
    vbd.data.resize(vertices.size() * sizeof(TVertex));
    std::memcpy(vbd.data.data(), vertices.data(), vbd.data.size());
    return vbd;
}

Vector<VertexAttributeDesc> MakePNTAttributes()
{
    return
    {
        { VertexAttributeSemantic::Position, Format::R32G32B32_FLOAT, offsetof(GenVertex, position) },
        { VertexAttributeSemantic::Normal,   Format::R32G32B32_FLOAT, offsetof(GenVertex, normal)   },
        { VertexAttributeSemantic::TexCoord, Format::R32G32_FLOAT,    offsetof(GenVertex, texCoord) },
    };
}

} // namespace

// ---------------------------------------------------------------------------
// Cube
// ---------------------------------------------------------------------------

GeometryData GeometryGenerator::GenerateCube(float size)
{
    const float h = size * 0.5f;

    // Each face has 4 vertices with its own normal and UV — no shared edges.
    // Vertex order per face is CCW when viewed from outside (outward normals).
    struct FaceVertex { Math::Vector3 pos; Math::Vector3 nrm; Math::Vector2 uv; };
    struct Face       { FaceVertex verts[4]; };

    const Face faces[6] =
    {
        // +Z
        { { { {-h,-h, h}, {0,0,1}, {0,0} }, { { h,-h, h}, {0,0,1}, {1,0} },
            { { h, h, h}, {0,0,1}, {1,1} }, { {-h, h, h}, {0,0,1}, {0,1} } } },
        // -Z
        { { { { h,-h,-h}, {0,0,-1}, {0,0} }, { {-h,-h,-h}, {0,0,-1}, {1,0} },
            { {-h, h,-h}, {0,0,-1}, {1,1} }, { { h, h,-h}, {0,0,-1}, {0,1} } } },
        // +Y
        { { { {-h, h, h}, {0,1,0}, {0,0} }, { { h, h, h}, {0,1,0}, {1,0} },
            { { h, h,-h}, {0,1,0}, {1,1} }, { {-h, h,-h}, {0,1,0}, {0,1} } } },
        // -Y
        { { { {-h,-h,-h}, {0,-1,0}, {0,0} }, { { h,-h,-h}, {0,-1,0}, {1,0} },
            { { h,-h, h}, {0,-1,0}, {1,1} }, { {-h,-h, h}, {0,-1,0}, {0,1} } } },
        // +X
        { { { { h,-h, h}, {1,0,0}, {0,0} }, { { h,-h,-h}, {1,0,0}, {1,0} },
            { { h, h,-h}, {1,0,0}, {1,1} }, { { h, h, h}, {1,0,0}, {0,1} } } },
        // -X
        { { { {-h,-h,-h}, {-1,0,0}, {0,0} }, { {-h,-h, h}, {-1,0,0}, {1,0} },
            { {-h, h, h}, {-1,0,0}, {1,1} }, { {-h, h,-h}, {-1,0,0}, {0,1} } } },
    };

    Vector<GenVertex> verts;
    verts.reserve(24);

    Vector<uint32> indices;
    indices.reserve(36);

    for (uint32 f = 0; f < 6; ++f)
    {
        const uint32 base = f * 4;
        for (const auto& fv : faces[f].verts)
            verts.push_back({ fv.pos, fv.nrm, fv.uv });

        // Two CCW triangles per quad
        indices.push_back(base + 0); indices.push_back(base + 1); indices.push_back(base + 2);
        indices.push_back(base + 0); indices.push_back(base + 2); indices.push_back(base + 3);
    }

    return { PackVertices(std::move(verts), MakePNTAttributes()), std::move(indices) };
}

// ---------------------------------------------------------------------------
// Sphere
// ---------------------------------------------------------------------------

GeometryData GeometryGenerator::GenerateSphere(float radius, uint32 slices, uint32 stacks)
{
    Vector<GenVertex> verts;
    verts.reserve((slices + 1) * (stacks + 1));

    Vector<uint32> indices;
    indices.reserve(slices * stacks * 6);

    // phi   — polar angle: 0 (north pole) → π (south pole)
    // theta — azimuthal angle: 0 → 2π
    for (uint32 stack = 0; stack <= stacks; ++stack)
    {
        const float phi    = std::numbers::pi_v<float> * static_cast<float>(stack) / static_cast<float>(stacks);
        const float sinPhi = std::sin(phi);
        const float cosPhi = std::cos(phi);

        for (uint32 slice = 0; slice <= slices; ++slice)
        {
            const float theta = 2.0f * std::numbers::pi_v<float> * static_cast<float>(slice) / static_cast<float>(slices);

            const Math::Vector3 normal =
            {
                sinPhi * std::cos(theta),
                cosPhi,
                sinPhi * std::sin(theta),
            };

            verts.push_back(
            {
                normal * radius,
                normal,
                { static_cast<float>(slice) / static_cast<float>(slices),
                  static_cast<float>(stack) / static_cast<float>(stacks) },
            });
        }
    }

    for (uint32 stack = 0; stack < stacks; ++stack)
    {
        for (uint32 slice = 0; slice < slices; ++slice)
        {
            const uint32 a = stack * (slices + 1) + slice;
            const uint32 b = a + (slices + 1);

            // CCW, outward normals: (a, a+1, b) and (a+1, b+1, b)
            indices.push_back(a);     indices.push_back(a + 1); indices.push_back(b);
            indices.push_back(a + 1); indices.push_back(b + 1); indices.push_back(b);
        }
    }

    return { PackVertices(std::move(verts), MakePNTAttributes()), std::move(indices) };
}

// ---------------------------------------------------------------------------
// Toroid
// ---------------------------------------------------------------------------

GeometryData GeometryGenerator::GenerateToroid(float outerRadius, float innerRadius,
                                                uint32 sides, uint32 rings)
{
    Vector<GenVertex> verts;
    verts.reserve((rings + 1) * (sides + 1));

    Vector<uint32> indices;
    indices.reserve(rings * sides * 6);

    // theta — angle around the main (Y) axis of the torus
    // phi   — angle around the tube cross-section
    for (uint32 ring = 0; ring <= rings; ++ring)
    {
        const float theta    = 2.0f * std::numbers::pi_v<float> * static_cast<float>(ring) / static_cast<float>(rings);
        const float cosTheta = std::cos(theta);
        const float sinTheta = std::sin(theta);

        for (uint32 side = 0; side <= sides; ++side)
        {
            const float phi    = 2.0f * std::numbers::pi_v<float> * static_cast<float>(side) / static_cast<float>(sides);
            const float cosPhi = std::cos(phi);
            const float sinPhi = std::sin(phi);

            verts.push_back(
            {
                {
                    (outerRadius + innerRadius * cosPhi) * cosTheta,
                    innerRadius * sinPhi,
                    (outerRadius + innerRadius * cosPhi) * sinTheta,
                },
                {
                    cosPhi * cosTheta,
                    sinPhi,
                    cosPhi * sinTheta,
                },
                {
                    static_cast<float>(ring) / static_cast<float>(rings),
                    static_cast<float>(side) / static_cast<float>(sides),
                },
            });
        }
    }

    for (uint32 ring = 0; ring < rings; ++ring)
    {
        for (uint32 side = 0; side < sides; ++side)
        {
            const uint32 a = ring * (sides + 1) + side;
            const uint32 b = a + (sides + 1);

            // CCW, outward normals: (a, a+1, b) and (a+1, b+1, b)
            indices.push_back(a);     indices.push_back(a + 1); indices.push_back(b);
            indices.push_back(a + 1); indices.push_back(b + 1); indices.push_back(b);
        }
    }

    return { PackVertices(std::move(verts), MakePNTAttributes()), std::move(indices) };
}

} // namespace pdl
