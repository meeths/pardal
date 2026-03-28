
#ifdef PDL_FEATURE_IMGUI

#include "Render/Passes/EditorRenderGraphPass.h"

#include "Geometry/GeometryGenerator.h"
#include "Log/Log.h"
#include "Math/Matrix44.h"
#include "Render/RenderTargetCache.h"
#include "Renderer/RHIDescriptors.h"
#include "Renderer/Shaders/SlangShaderCompiler.h"
#include "Renderer/RendererTypes.h"

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

// Created on 2026-03-28 by Sisco

namespace pdl
{

// ---------------------------------------------------------------------------
// Shader sources
// ---------------------------------------------------------------------------

static constexpr const char* kVertexShaderSource = R"(
struct PushConstants
{
    float4x4 transform;
    float4   color;
};

[[vk::push_constant]] PushConstants pc;

[shader("vertex")]
float4 vertMain(float3 pos : POSITION) : SV_Position
{
    return mul(pc.transform, float4(pos, 1.0));
}
)";

static constexpr const char* kFragmentShaderSource = R"(
struct PushConstants
{
    float4x4 transform;
    float4   color;
};

[[vk::push_constant]] PushConstants pc;

[shader("fragment")]
float4 fragMain() : SV_Target
{
    return pc.color;
}
)";

// ---------------------------------------------------------------------------
// Push-constant layout (must match the shader structs above)
// ---------------------------------------------------------------------------

struct WireframePushConstants
{
    Math::Matrix44 transform;
    Math::Vector4  color;
};

// ---------------------------------------------------------------------------
// Setup
// ---------------------------------------------------------------------------

void EditorRenderGraphPass::Setup(RenderGraphPassBuilder& builder)
{
    // Declare the shared depth target (auto-sized to swapchain dimensions).
    builder.DeclareTarget(DepthTargetName, TextureDesc{
        .type   = TextureType::Texture2D,
        .format = Format::D32_FLOAT,
        .width  = 0,
        .height = 0,
        .usage  = TextureUsage::DepthWrite | TextureUsage::DepthRead,
    });

    // Clear the swapchain to the engine background colour.
    builder.WriteColorTarget({
        .targetName = String(RenderTargetCache::Swapchain),
        .loadOp     = LoadOp::Clear,
        .storeOp    = StoreOp::Store,
        .clearColor = {0.08f, 0.08f, 0.10f, 1.0f},
    });

    // Clear the depth buffer to 1.0 (far plane).
    builder.WriteDepthTarget({
        .targetName = String(DepthTargetName),
        .loadOp     = LoadOp::Clear,
        .storeOp    = StoreOp::Store,
        .clearDepth = 1.0f,
    });
}

// ---------------------------------------------------------------------------
// Resource initialisation (called once from Execute)
// ---------------------------------------------------------------------------

void EditorRenderGraphPass::InitializeResources(IRHIContext& rhi)
{
    // Compile shaders via Slang.
    SlangShaderCompiler compiler({
        .m_target          = SlangShaderCompiler::Target::SPIRV,
        .m_compilerOptions = SlangShaderCompiler::CompilerOptions::TargetVulkan,
    });

    auto compileVS = compiler.CompileShader({
        .m_shaderSourceFile = "EditorPass_VS",
        .m_shaderSource     = kVertexShaderSource,
        .m_entryPoint       = "vertMain",
    });
    if (!compileVS)
    {
        pdlLogError("EditorRenderGraphPass: vertex shader compile error: %s", compileVS.error().c_str());
        return;
    }

    auto compileFS = compiler.CompileShader({
        .m_shaderSourceFile = "EditorPass_FS",
        .m_shaderSource     = kFragmentShaderSource,
        .m_entryPoint       = "fragMain",
    });
    if (!compileFS)
    {
        pdlLogError("EditorRenderGraphPass: fragment shader compile error: %s", compileFS.error().c_str());
        return;
    }

    auto vsResult = rhi.CreateShaderModule({
        .spirvData = compileVS->data(),
        .spirvSize = compileVS->size(),
        .debugName = "EditorPass_VS",
    });
    if (!vsResult)
    {
        pdlLogError("EditorRenderGraphPass: failed to create vertex shader module: %s", vsResult.error().c_str());
        return;
    }
    m_vertexShader = {&rhi, *vsResult};

    auto fsResult = rhi.CreateShaderModule({
        .spirvData = compileFS->data(),
        .spirvSize = compileFS->size(),
        .debugName = "EditorPass_FS",
    });
    if (!fsResult)
    {
        pdlLogError("EditorRenderGraphPass: failed to create fragment shader module: %s", fsResult.error().c_str());
        return;
    }
    m_fragmentShader = {&rhi, *fsResult};

    // Build geometry: indexed cube from GeometryGenerator.
    GeometryData cubeData = GeometryGenerator::GenerateCube(1.0f);

    auto meshResult = Mesh::Create(rhi, cubeData);
    if (!meshResult)
    {
        pdlLogError("EditorRenderGraphPass: failed to create cube mesh: %s", meshResult.error().c_str());
        return;
    }
    m_cubeMesh = std::move(*meshResult);

    // Build the wireframe render pipeline.
    RenderPipelineDesc pipelineDesc;
    pipelineDesc.vertexShader   = m_vertexShader;
    pipelineDesc.fragmentShader = m_fragmentShader;
    pipelineDesc.vertexInput    = cubeData.vertexBuffer.BuildVertexInput(0);
    pipelineDesc.topology       = PrimitiveTopology::TriangleList;
    pipelineDesc.polygonMode    = PolygonMode::Line;
    pipelineDesc.cullMode       = CullMode::None;
    pipelineDesc.frontFace      = FrontFace::CounterClockwise;
    pipelineDesc.depthTest      = {.m_enabled = true, .m_writeEnabled = true, .m_compareOp = CompareOp::LessOrEqual};
    pipelineDesc.depthFormat    = Format::D32_FLOAT;
    pipelineDesc.colorAttachments[0] = {.format = rhi.GetSwapchainFormat()};
    pipelineDesc.numColorAttachments = 1;
    pipelineDesc.debugName      = "EditorPass_Wireframe";

    auto pipelineResult = rhi.CreateRenderPipeline(pipelineDesc);
    if (!pipelineResult)
    {
        pdlLogError("EditorRenderGraphPass: failed to create pipeline: %s", pipelineResult.error().c_str());
        return;
    }
    m_pipeline = {&rhi, *pipelineResult};

    m_initialized = true;
}

// ---------------------------------------------------------------------------
// Execute
// ---------------------------------------------------------------------------

void EditorRenderGraphPass::Execute(RenderGraphPassContext& ctx)
{
#ifdef PDL_VULKAN
    if (!m_initialized)
        InitializeResources(ctx.rhi);

    if (!m_pipeline.IsValid() || !m_cubeMesh.HasIndices())
        return;

    ctx.cmd.CmdBeginRendering(ctx.renderPass, ctx.framebuffer);

    // Set viewport and scissor to the swapchain dimensions.
    const auto& colorTex = ctx.framebuffer.m_colorAttachments[0].m_texture;
    const auto  dims     = ctx.rhi.GetDimensions(colorTex);

    ctx.cmd.CmdSetViewport({
        .x = 0.0f, .y = 0.0f,
        .width  = static_cast<float>(dims.width),
        .height = static_cast<float>(dims.height),
        .minDepth = 0.0f, .maxDepth = 1.0f,
    });
    ctx.cmd.CmdSetScissorRect({
        .x = 0, .y = 0,
        .width  = dims.width,
        .height = dims.height,
    });

    ctx.cmd.CmdBindRenderPipeline(m_pipeline);

    // Simple MVP: perspective camera looking at the cube from slightly above.
    const float aspect = static_cast<float>(dims.width) / static_cast<float>(dims.height);
    const Math::Matrix44 proj = glm::perspective(glm::radians(60.0f), aspect, 0.1f, 100.0f);
    const Math::Matrix44 view = glm::lookAt(
        glm::vec3(1.5f, 1.5f, 3.0f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f));
    const Math::Matrix44 model = glm::mat4(1.0f);

    WireframePushConstants pc;
    pc.transform = proj * view * model;
    pc.color     = {1.0f, 0.8f, 0.2f, 1.0f}; // amber wireframe

    ctx.cmd.CmdPushConstants(&pc, sizeof(pc), 0);

    m_cubeMesh.Bind(ctx.cmd);
    m_cubeMesh.DrawIndexed(ctx.cmd);

    ctx.cmd.CmdEndRendering();
#endif
}

} // namespace pdl

#endif // PDL_FEATURE_IMGUI
