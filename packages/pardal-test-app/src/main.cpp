
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
#include "Renderer/ICommandBuffer.h"
#include "Renderer/RenderererDevices.h"
#include "Renderer/Shaders/device_host_structs.h"
#include "Renderer/Vulkan/VulkanRenderer.h"
#include "Renderer/Vulkan/lvk/HelpersImGui.h"
#include "String/StringUtils.h"
#include "Time/Chronometer.h"

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
    pdl::Memory::Initialize();
#ifndef PDL_RELEASE
    pdl::Log::Instance().RegisterLogger(pdl::MakeSharedPointer<pdl::LoggerStdout>());
#endif

    pdl::IApplicationWindow::InitInfoBase windowInitInfo;
    windowInitInfo.m_windowTitle = "pardal-test-app";

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
    auto* context = ((pdl::VulkanRenderer*)renderer.get())->GetLVKContext();
    
    lvk::Result outResult;
    lvk::Holder<lvk::ShaderModuleHandle> vert_ = context->createShaderModule({codeSlang, lvk::Stage_Vert, "Shader Module: main (vert)"}, &outResult);
    lvk::Holder<lvk::ShaderModuleHandle> frag_ = context->createShaderModule({codeSlang, lvk::Stage_Frag, "Shader Module: main (frag)"}, &outResult);

    lvk::Holder<lvk::RenderPipelineHandle> renderPipelineState_Triangle_ = context->createRenderPipeline(
    {
        .smVert = vert_,
        .smFrag = frag_,
        .color = {{.format = context->getSwapchainFormat()}},
    },
    &outResult);

    LVK_ASSERT(renderPipelineState_Triangle_.valid());

    
    pdl::Chronometer frameTimer;
    frameTimer.Start();
    
    pdl::ImGuiPerfWidget perfWidget;

    pdl::InputManager inputManager;
    
    while (!window.IsCloseRequested())
    {
        float deltaTime = frameTimer.Lap<float, pdl::TimeTypes::Seconds>();
        perfWidget.Update(deltaTime, 0);
        auto inputManagerResults = inputManager.Update();
        window.Update();
        
        if (!inputManagerResults)
        {
            pdlLogError("%s", inputManagerResults.error().c_str());
        }

      
        auto& cmd = context->acquireCommandBuffer();
        lvk::Framebuffer framebuffer{
                .color = {{.texture = context->getCurrentSwapchainTexture()}}
        };
        cmd.cmdBeginRendering({.color = {{.loadOp = lvk::LoadOp_Clear, .clearColor = {{1.0f, 0.0f, 1.0f, 0.0f}}}}},
                                 framebuffer);
        
        cmd.cmdBindRenderPipeline(renderPipelineState_Triangle_);
        cmd.cmdDraw(3);

        auto& imguiRenderer  = pdl::ServiceLocator<pdl::ImGuiRenderer>::Ref();
        
        imguiRenderer.GetRenderer().beginFrame(framebuffer);
        imguiRenderer.Render();
        imguiRenderer.GetRenderer().endFrame(cmd);
        cmd.cmdEndRendering();
        context->submit(cmd, context->getCurrentSwapchainTexture());

        
        pdlLogFlush();
    }
    
    return 0;
    
}
