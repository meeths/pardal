
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
#include "Renderer/RenderererDevices.h"
#include "Renderer/Shaders/device_host_structs.h"
#include "Renderer/Vulkan/VulkanRenderer.h"
#include "Renderer/Vulkan/lvk/HelpersImGui.h"
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

      
        auto& cmd = context->acquireCommandBuffer();
        lvk::Framebuffer framebuffer{
                .color = {{.texture = context->getCurrentSwapchainTexture()}}
        };
        cmd.cmdBeginRendering({.color = {{.loadOp = lvk::LoadOp_Clear, .clearColor = {{1.0f, 0.0f, 1.0f, 0.0f}}}}},
                                 framebuffer);
        
        {
            pdlProfileScopedN("Triangle rendering");
            cmd.cmdPushDebugGroupLabel("Triangle");
            cmd.cmdBindRenderPipeline(renderPipelineState_Triangle_);
            cmd.cmdDraw(3);
            cmd.cmdPopDebugGroupLabel();
        }

        auto& imguiRenderer  = pdl::ServiceLocator<pdl::ImGuiRenderer>::Ref();

        {
            pdlProfileScopedN("ImGui rendering");
            imguiRenderer.GetRenderer().beginFrame(framebuffer);
            imguiRenderer.Render();
            imguiRenderer.GetRenderer().endFrame(cmd);
        }
        {
            pdlProfileScopedN("Submitting command buffer");
            cmd.cmdEndRendering();
            context->submit(cmd, context->getCurrentSwapchainTexture());
        }

        
        pdlLogFlush();
    }
    
    return 0;
    
}
