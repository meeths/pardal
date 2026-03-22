
#include "Application/ApplicationWindow.h"
#include "Base/DebugHelpers.h"
#include "Base/ServiceLocator.h"
#include "ImGui/ImGuiPerfWidget.h"
#include "ImGui/ImGuiRenderer.h"
#include "Input/InputManager.h"
#include "Log/Log.h"
#include "Log/LoggerStdout.h"
#include "Math/Vector3.h"
#include "Memory/Memory.h"
#include "Renderer/IRHICommandBuffer.h"
#include "Renderer/IRHIContext.h"
#include "Renderer/IRenderer.h"
#include "Renderer/RenderererDevices.h"
#include "Renderer/Shaders/SlangShaderCompiler.h"
#include "Renderer/Vulkan/VulkanRenderer.h"
#include "String/StringUtils.h"
#include "Time/Chronometer.h"

#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Engine/CoreSystems.h"
#include "Engine/EngineOptions.h"
#include "Profiling/Instrumentation.h"

const char* codeSlang = R"(
static const float2 pos[3] = float2[3](
  float2(-0.6, -0.4),
  float2( 0.6, -0.4),
  float2( 0.0,  0.6)
);
static const float3 col[3] = float3[3](
  float3(1.0, 0.0, 0.0),
  float3(0.0, 1.0, 0.0),
  float3(0.0, 0.0, 1.0)
);

struct VertexStageOutput {
  float4 sv_Position  : SV_Position;
  float3 color        : COLOR0;
};

[shader("vertex")]
VertexStageOutput vertexMain(uint vertexID : SV_VertexID) {
  return {
    float4(pos[vertexID], 0.0, 1.0),
    col[vertexID],
  };
}

[shader("fragment")]
float4 fragmentMain(float3 color : COLOR0) : SV_Target {
  return float4(color, 1.0);
}
)";

int main(int argc, char** argv)
{
    if (!pdl::CoreSystems::Initialize())
    {
        return -1;
    }
        
#ifndef PDL_RELEASE
    pdl::ServiceLocator<pdl::Log>::Ref().RegisterLogger(pdl::MakeSharedPointer<pdl::LoggerStdout>());
#endif

    pdl::IApplicationWindow::InitInfoBase windowInitInfo;
    windowInitInfo.m_windowTitle = "pardal-test-app";

    if (auto windowHandleOption = pdl::ServiceLocator<pdl::EngineOptions>::Ref().GetOption<unsigned long long>("parent_window"))
    {
        windowInitInfo.m_parentWindow = reinterpret_cast<void*>(windowHandleOption.value());
    }
		
    if (const auto windowRectOption =  pdl::ServiceLocator<pdl::EngineOptions>::Ref().GetOption<pdl::Math::Vector4>("window_rect"))
    {
        auto windowRect = windowRectOption.value();
        windowInitInfo.m_windowPosition = {windowRect.x, windowRect.y};
        windowInitInfo.m_windowSize = {windowRect.z, windowRect.w};
    }

    
    pdl::ApplicationWindow window(windowInitInfo);

    pdl::IRenderer::InitInfo rendererInitInfo
    {
        .m_applicationName = "pardal-test-app",
        .m_applicationWindow = window,
        .m_enableValidation = true,
        .m_useVSync = true,
        .m_useHDR = false
    };
    auto renderer = pdl::CreateRenderer(pdl::RenderDeviceType::Vulkan, rendererInitInfo);
    pdl::IRHIContext* rhi = renderer->GetRHIContext();

    // Compile shaders
    pdl::SlangShaderCompiler compiler({
        .m_target          = pdl::SlangShaderCompiler::Target::SPIRV,
        .m_compilerOptions = pdl::SlangShaderCompiler::CompilerOptions::TargetVulkan,
    });

    auto vertSpirv = compiler.CompileShader({ "main_vert", codeSlang, "vertexMain" });
    auto fragSpirv = compiler.CompileShader({ "main_frag", codeSlang, "fragmentMain" });
    pdlAssert(vertSpirv.has_value());
    pdlAssert(fragSpirv.has_value());

    auto vertResult = rhi->CreateShaderModule({ .spirvData = vertSpirv->data(), .spirvSize = vertSpirv->size(), .debugName = "main_vert" });
    auto fragResult = rhi->CreateShaderModule({ .spirvData = fragSpirv->data(), .spirvSize = fragSpirv->size(), .debugName = "main_frag" });
    pdlAssert(vertResult.has_value());
    pdlAssert(fragResult.has_value());

    pdl::Holder<pdl::ShaderModuleHandle> vert(rhi, *vertResult);
    pdl::Holder<pdl::ShaderModuleHandle> frag(rhi, *fragResult);

    pdl::RenderPipelineDesc pipelineDesc;
    pipelineDesc.vertexShader        = vert;
    pipelineDesc.fragmentShader      = frag;
    pipelineDesc.colorAttachments[0].format = rhi->GetSwapchainFormat();
    pipelineDesc.numColorAttachments = 1;
    pipelineDesc.debugName           = "Triangle";

    auto pipelineResult = rhi->CreateRenderPipeline(pipelineDesc);
    pdlAssert(pipelineResult.has_value());
    pdl::Holder<pdl::RenderPipelineHandle> trianglePipeline(rhi, *pipelineResult);


    pdl::Chronometer frameTimer;
    frameTimer.Start();

    pdl::ImGuiPerfWidget perfWidget;

    pdl::InputManager inputManager;

    pdlMaybeUnused auto scene = aiImportFile("Test",aiProcessPreset_TargetRealtime_MaxQuality);

    while (!window.IsCloseRequested())
    {
        pdlProfileScopedN("Main loop");

        float deltaTime = frameTimer.Lap<float, pdl::TimeTypes::Seconds>();
        perfWidget.Update(deltaTime, 0);
        auto inputManagerResults = inputManager.Update();
        window.Update();

        if (!inputManagerResults)
        {
            pdlLogError("%s", inputManagerResults.error().c_str());
        }

        pdl::TextureHandle swapchainTexture = rhi->GetCurrentSwapchainTexture();

        pdl::Framebuffer framebuffer;
        framebuffer.m_colorAttachments[0].m_texture = swapchainTexture;

        pdl::RenderPass renderPass;
        renderPass.m_colorAttachments[0].clearColor = { 1.0f, 0.0f, 1.0f, 0.0f };
        renderPass.m_colorAttachments[0].loadOp     = pdl::LoadOp::Clear;
        renderPass.m_colorAttachments[0].storeOp    = pdl::StoreOp::Store;

        auto& cmd = rhi->AcquireCommandBuffer();
        cmd.CmdBeginRendering(renderPass, framebuffer);

        {
            pdlProfileScopedN("Triangle rendering");
            cmd.CmdPushDebugGroupLabel("Triangle");
            cmd.CmdBindRenderPipeline(trianglePipeline);
            cmd.CmdDraw(3);
            cmd.CmdPopDebugGroupLabel();
        }

        auto& imguiRenderer = pdl::ServiceLocator<pdl::ImGuiRenderer>::Ref();

        {
            pdlProfileScopedN("ImGui rendering");
            imguiRenderer.BeginFrame(swapchainTexture);
            imguiRenderer.Render();
            imguiRenderer.EndFrame(cmd);
        }
        {
            pdlProfileScopedN("Submitting command buffer");
            cmd.CmdEndRendering();
            rhi->Submit(cmd, swapchainTexture);
        }


        pdlLogFlush();
    }

    return 0;

}
