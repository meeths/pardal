#include "Render/Passes/EditorGrid.h"

#include "Log/Log.h"
#include "Renderer/RHIDescriptors.h"
#include "Renderer/Shaders/SlangShaderCompiler.h"
#include "Renderer/RendererTypes.h"

#include <glm/gtc/matrix_inverse.hpp>

// Created on 2026-03-28 by Sisco

namespace pdl
{

// ---------------------------------------------------------------------------
// Shader sources
// ---------------------------------------------------------------------------

static constexpr const char* kGridVertexShaderSource = R"(
struct PushConstants
{
    float4x4 invVP;     // inverse(proj * view) — unprojects NDC to world space
    float4x4 VP;        // proj * view — reprojects world pos to clip for depth
    float4   cameraPos; // xyz = camera world position, w unused
    float4   params1;   // x=cellSize, y=coarseFactor, z=lineWidth, w=axisLineWidth
    float4   params2;   // x=fadeFar,  y=fadeNear,     zw=unused
    float4   minorColor;
    float4   majorColor;
    float4   xAxisColor;
    float4   zAxisColor;
};

[[vk::push_constant]] PushConstants pc;

struct VsOut
{
    float4 clipPos : SV_Position;
    float3 nearPos : NEAR;  // world-space position on the near plane
    float3 farPos  : FAR;   // world-space position on the far plane
};

[shader("vertex")]
VsOut vertMain(uint vid : SV_VertexID)
{
    // Full-screen triangle: 3 vertices cover the entire viewport without a VB.
    // vid=0 -> (-1,-1), vid=1 -> (3,-1), vid=2 -> (-1,3)
    float2 uv  = float2((vid << 1) & 2, vid & 2);
    float2 ndc = uv * 2.0 - 1.0;

    VsOut o;
    o.clipPos = float4(ndc, 0.0, 1.0);

    // Unproject the NDC point at the near (z=0) and far (z=1) Vulkan planes.
    // mul(v, M) is the correct convention for GLM column-major matrices passed
    // into HLSL row-major float4x4.
    float4 nearH = mul(float4(ndc, 0.0, 1.0), pc.invVP);
    float4 farH  = mul(float4(ndc, 1.0, 1.0), pc.invVP);
    o.nearPos = nearH.xyz / nearH.w;
    o.farPos  = farH.xyz  / farH.w;
    return o;
}
)";

// ---------------------------------------------------------------------------

static constexpr const char* kGridFragmentShaderSource = R"(
struct PushConstants
{
    float4x4 invVP;
    float4x4 VP;
    float4   cameraPos;
    float4   params1;
    float4   params2;
    float4   minorColor;
    float4   majorColor;
    float4   xAxisColor;
    float4   zAxisColor;
};

[[vk::push_constant]] PushConstants pc;

struct FragOut
{
    float4 color : SV_Target;
    float  depth : SV_Depth;
};

// Anti-aliased grid alpha for a given world-space XZ position and cell scale.
// lineWidthPx widens the line by scaling the derivative threshold.
float gridAlpha(float2 xz, float scale, float lineWidthPx)
{
    float2 coord = xz / scale;
    float2 fw    = max(fwidth(coord), 1e-6);
    float2 g     = abs(frac(coord - 0.5) - 0.5) / (fw * lineWidthPx);
    return 1.0 - saturate(min(g.x, g.y));
}

// Porter-Duff "over": blend src on top of dst using src.a.
float4 blendOver(float4 dst, float4 src)
{
    return float4(lerp(dst.rgb, src.rgb, src.a), max(dst.a, src.a));
}

[shader("fragment")]
FragOut fragMain(float3 nearPos : NEAR, float3 farPos : FAR)
{
    FragOut o;
    o.color = float4(0.0, 0.0, 0.0, 0.0);
    o.depth = 1.0;

    // ── Ray–Y=0 plane intersection ─────────────────────────────────────────
    float denom = farPos.y - nearPos.y;
    if (abs(denom) < 1e-6) { discard; return o; }

    float t = -nearPos.y / denom;
    if (t <= 0.0) { discard; return o; }   // intersection is behind the camera

    float3 worldPos = nearPos + t * (farPos - nearPos);
    float2 xz       = worldPos.xz;

    // ── Unpack parameters ───────────────────────────────────────────────────
    float  cellSize     = pc.params1.x;
    float  coarseFactor = pc.params1.y;
    float  lineWidth    = pc.params1.z;
    float  axisLineWid  = pc.params1.w;
    float  fadeFar      = pc.params2.x;
    float  fadeNear     = pc.params2.y;
    float2 camXZ        = pc.cameraPos.xz;

    // ── Distance fade ───────────────────────────────────────────────────────
    float dist    = length(xz - camXZ);
    float farFade = 1.0 - smoothstep(fadeFar * 0.5, fadeFar, dist);
    float nearFade = smoothstep(0.0, fadeNear, dist);
    float fade    = farFade * nearFade;
    if (fade < 0.001) { discard; return o; }

    // ── Grid LOD ─────────────────────────────────────────────────────────────
    // Fine grid at cellSize; coarse grid at cellSize*coarseFactor.
    float fineAlpha   = gridAlpha(xz, cellSize,                lineWidth);
    float coarseAlpha = gridAlpha(xz, cellSize * coarseFactor, lineWidth);

    // Smoothly fade the fine grid out as its cells approach sub-pixel size,
    // leaving only the coarse grid visible when zoomed far out.
    float cellPx = cellSize / max(length(fwidth(xz)), 1e-9);
    fineAlpha   *= saturate(cellPx - 1.0);

    // ── Axis lines ──────────────────────────────────────────────────────────
    // X axis runs along xz.x (z == 0); Z axis runs along xz.z (x == 0).
    float2 fw         = max(fwidth(xz), 1e-6);
    float  xAxisAlpha = 1.0 - saturate(abs(xz.y) / (fw.y * axisLineWid));
    float  zAxisAlpha = 1.0 - saturate(abs(xz.x) / (fw.x * axisLineWid));

    // ── Composite: axis > coarse > fine ─────────────────────────────────────
    float4 color = float4(0.0, 0.0, 0.0, 0.0);
    color = blendOver(color, float4(pc.minorColor.rgb, fineAlpha   * pc.minorColor.a));
    color = blendOver(color, float4(pc.majorColor.rgb, coarseAlpha * pc.majorColor.a));
    color = blendOver(color, float4(pc.xAxisColor.rgb, xAxisAlpha  * pc.xAxisColor.a));
    color = blendOver(color, float4(pc.zAxisColor.rgb, zAxisAlpha  * pc.zAxisColor.a));

    color.a *= fade;
    if (color.a < 0.001) { discard; return o; }

    // ── Depth ────────────────────────────────────────────────────────────────
    // Reproject the world intersection point to get the correct clip-space
    // depth, so the grid occludes and is occluded by scene geometry.
    float4 clipPos = mul(float4(worldPos, 1.0), pc.VP);
    o.depth = clipPos.z / clipPos.w;
    o.color = color;
    return o;
}
)";

// ---------------------------------------------------------------------------
// Push-constant layout
//
// All members are float4 or float4x4 to guarantee identical layout between
// C++ and SPIR-V std430 — no implicit padding surprises.
// Total: 240 bytes. Requires 256-byte push constant support, which is
// universal on PC Vulkan drivers (NVIDIA, AMD, Intel).
// ---------------------------------------------------------------------------

struct GridPushConstants
{
    Math::Matrix44 invVP;       // offset   0 — 64 bytes
    Math::Matrix44 VP;          // offset  64 — 64 bytes
    Math::Vector4  cameraPos;   // offset 128 — xyz = cam pos, w unused
    Math::Vector4  params1;     // offset 144 — cellSize, coarseFactor, lineWidth, axisLineWidth
    Math::Vector4  params2;     // offset 160 — fadeFar, fadeNear, 0, 0
    Math::Vector4  minorColor;  // offset 176
    Math::Vector4  majorColor;  // offset 192
    Math::Vector4  xAxisColor;  // offset 208
    Math::Vector4  zAxisColor;  // offset 224
    // Total: 240 bytes
};

static_assert(sizeof(GridPushConstants) == 240,
    "GridPushConstants size changed — re-verify std430 alignment with the Slang structs");

// ---------------------------------------------------------------------------
// Initialize
// ---------------------------------------------------------------------------

void EditorGrid::Initialize(IRHIContext& rhi, Format swapchainFormat)
{
    SlangShaderCompiler compiler({
        .m_target          = SlangShaderCompiler::Target::SPIRV,
        .m_compilerOptions = SlangShaderCompiler::CompilerOptions::TargetVulkan,
    });

    auto compileVS = compiler.CompileShader({
        .m_shaderSourceFile = "EditorGrid_VS",
        .m_shaderSource     = kGridVertexShaderSource,
        .m_entryPoint       = "vertMain",
    });
    if (!compileVS)
    {
        pdlLogError("EditorGrid: vertex shader compile error: %s", compileVS.error().c_str());
        return;
    }

    auto compileFS = compiler.CompileShader({
        .m_shaderSourceFile = "EditorGrid_FS",
        .m_shaderSource     = kGridFragmentShaderSource,
        .m_entryPoint       = "fragMain",
    });
    if (!compileFS)
    {
        pdlLogError("EditorGrid: fragment shader compile error: %s", compileFS.error().c_str());
        return;
    }

    auto vsResult = rhi.CreateShaderModule({
        .spirvData = compileVS->data(),
        .spirvSize = compileVS->size(),
        .debugName = "EditorGrid_VS",
    });
    if (!vsResult)
    {
        pdlLogError("EditorGrid: failed to create vertex shader module: %s", vsResult.error().c_str());
        return;
    }
    m_vertexShader = {&rhi, *vsResult};

    auto fsResult = rhi.CreateShaderModule({
        .spirvData = compileFS->data(),
        .spirvSize = compileFS->size(),
        .debugName = "EditorGrid_FS",
    });
    if (!fsResult)
    {
        pdlLogError("EditorGrid: failed to create fragment shader module: %s", fsResult.error().c_str());
        return;
    }
    m_fragmentShader = {&rhi, *fsResult};

    // Alpha blend: src_alpha over one_minus_src_alpha — standard transparency.
    BlendMode blend;
    blend.m_enabled  = true;
    blend.m_equation = {
        .m_srcFactor      = BlendMode::BlendFactor::SrcAlpha,
        .m_dstFactor      = BlendMode::BlendFactor::OneMinusSrcAlpha,
        .m_op             = BlendMode::BlendOp::Add,
        .m_srcFactorAlpha = BlendMode::BlendFactor::One,
        .m_dstFactorAlpha = BlendMode::BlendFactor::OneMinusSrcAlpha,
        .m_opAlpha        = BlendMode::BlendOp::Add,
    };

    RenderPipelineDesc pipelineDesc;
    pipelineDesc.vertexShader        = m_vertexShader;
    pipelineDesc.fragmentShader      = m_fragmentShader;
    pipelineDesc.vertexInput         = {};                        // no vertex buffer — SV_VertexID only
    pipelineDesc.topology            = PrimitiveTopology::TriangleList;
    pipelineDesc.polygonMode         = PolygonMode::Fill;
    pipelineDesc.cullMode            = CullMode::None;
    pipelineDesc.frontFace           = FrontFace::CounterClockwise;
    pipelineDesc.depthTest           = {.m_enabled = true, .m_writeEnabled = true, .m_compareOp = CompareOp::LessOrEqual};
    pipelineDesc.depthFormat         = Format::D32_FLOAT;
    pipelineDesc.colorAttachments[0] = {.format = swapchainFormat, .blendMode = blend};
    pipelineDesc.numColorAttachments = 1;
    pipelineDesc.debugName           = "EditorGrid";

    auto pipelineResult = rhi.CreateRenderPipeline(pipelineDesc);
    if (!pipelineResult)
    {
        pdlLogError("EditorGrid: failed to create pipeline: %s", pipelineResult.error().c_str());
        return;
    }
    m_pipeline    = {&rhi, *pipelineResult};
    m_initialized = true;
}

// ---------------------------------------------------------------------------
// Render
// ---------------------------------------------------------------------------

void EditorGrid::Render(IRHICommandBuffer& cmd,
                        const Math::Matrix44& view,
                        const Math::Matrix44& proj,
                        const Math::Vector3&  cameraPos,
                        const GridConfig&     config)
{
#ifdef PDL_VULKAN
    if (!m_initialized)
        return;

    const Math::Matrix44 VP    = proj * view;
    const Math::Matrix44 invVP = glm::inverse(VP);

    GridPushConstants pc;
    pc.invVP      = invVP;
    pc.VP         = VP;
    pc.cameraPos  = Math::Vector4(cameraPos, 0.0f);
    pc.params1    = {config.cellSize, config.coarseFactor, config.lineWidth, config.axisLineWidth};
    pc.params2    = {config.fadeFar,  config.fadeNear,     0.0f,            0.0f};
    pc.minorColor = config.minorColor;
    pc.majorColor = config.majorColor;
    pc.xAxisColor = config.xAxisColor;
    pc.zAxisColor = config.zAxisColor;

    cmd.CmdBindRenderPipeline(m_pipeline);
    cmd.CmdPushConstants(&pc, sizeof(pc), 0);
    cmd.CmdDraw(3, 1, 0, 0);
#endif
}

} // namespace pdl
