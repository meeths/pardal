#pragma once
#include "Base/BaseTypes.h"
#include "Geometry/GeometryData.h"

// Created on 2026-03-20 by sisco

namespace pdl
{

class GeometryGenerator
{
public:
    // Axis-aligned cube centred at the origin. Each face has its own 4 vertices
    // (so normals and UVs are face-local — no shared edges).
    [[nodiscard]] static GeometryData GenerateCube(float size = 1.0f);

    // UV sphere centred at the origin.
    //   slices — number of longitudinal segments (around Y axis)
    //   stacks — number of latitudinal segments (pole to pole)
    [[nodiscard]] static GeometryData GenerateSphere(float radius = 0.5f, uint32 slices = 32, uint32 stacks = 16);

    // Toroid (donut) centred at the origin, lying in the XZ plane.
    //   outerRadius — distance from the centre of the tube to the centre of the torus
    //   innerRadius — radius of the tube cross-section
    //   sides       — segments around the tube cross-section
    //   rings       — segments around the main axis
    [[nodiscard]] static GeometryData GenerateToroid(float outerRadius = 0.5f, float innerRadius = 0.2f,
                                                     uint32 sides = 32, uint32 rings = 32);
};

} // namespace pdl
