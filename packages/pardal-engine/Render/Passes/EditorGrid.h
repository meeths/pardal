
#pragma once

#include "Math/Matrix44.h"
#include "Math/Vector3.h"
#include "Math/Vector4.h"
#include "Renderer/IRHICommandBuffer.h"
#include "Renderer/IRHIContext.h"
#include "Renderer/RendererTypes.h"

// Created on 2026-03-28 by Sisco

namespace pdl
{

// Controls the appearance of the editor floor grid.
// All parameters are uploaded as push constants each frame and can be changed at runtime.
struct GridConfig
{
    // Geometry
    float cellSize      = 1.0f;    // World-unit size of one fine grid cell
    float coarseFactor  = 10.0f;   // Coarse grid cell = cellSize * coarseFactor

    // Line appearance
    float lineWidth     = 1.5f;    // Anti-aliased line half-width in pixels (fine and coarse)
    float axisLineWidth = 2.5f;    // Axis line half-width in pixels

    // Distance fading
    float fadeFar       = 80.0f;   // Distance at which the grid fully fades out
    float fadeNear      = 0.5f;    // Fade-in radius immediately around the camera eye

    // Colors — the alpha channel scales each layer's maximum opacity
    Math::Vector4 minorColor = {0.35f, 0.35f, 0.35f, 1.0f};  // Fine grid lines
    Math::Vector4 majorColor = {0.60f, 0.60f, 0.60f, 1.0f};  // Coarse grid lines
    Math::Vector4 xAxisColor = {0.80f, 0.20f, 0.20f, 1.0f};  // X axis (red)
    Math::Vector4 zAxisColor = {0.20f, 0.45f, 0.80f, 1.0f};  // Z axis (blue)
};

// Renders an infinite anti-aliased floor grid on the Y=0 world plane.
//
// Technique: a single full-screen triangle with no vertex buffer. The fragment
// shader intersects the per-pixel view ray with the Y=0 plane, computes grid
// lines at two LOD levels with fwidth()-based anti-aliasing, blends coloured
// axis lines on top, applies smooth near/far distance fading, and outputs a
// corrected SV_Depth so the grid interacts correctly with scene geometry.
class EditorGrid
{
public:
    // Compile shaders and build the pipeline. swapchainFormat must match the
    // colour attachment the grid will be rendered into.
    void Initialize(IRHIContext& rhi, Format swapchainFormat);

    // Record the grid draw into cmd. view/proj must follow the Vulkan convention
    // used by the editor pass (perspectiveRH_ZO, Y-flipped projection).
    void Render(IRHICommandBuffer& cmd,
                const Math::Matrix44& view,
                const Math::Matrix44& proj,
                const Math::Vector3&  cameraPos,
                const GridConfig&     config);

    bool IsInitialized() const { return m_initialized; }

private:
    bool                         m_initialized  = false;
    Holder<ShaderModuleHandle>   m_vertexShader;
    Holder<ShaderModuleHandle>   m_fragmentShader;
    Holder<RenderPipelineHandle> m_pipeline;
};

} // namespace pdl
