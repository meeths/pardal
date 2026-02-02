
#include "Application/ApplicationWindow.h"
#include "Base/DebugHelpers.h"
#include "ImGui/ImGuiPerfWidget.h"
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
#include "Renderer/Vulkan/lvk/vulkan/VulkanClasses.h"
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
    //auto renderer = pdl::CreateRenderer(pdl::RenderDeviceType::Vulkan, rendererInitInfo);

    lvk::ContextConfig contextConfig;
    pdl::UniquePointer<lvk::VulkanContext> context = pdl::MakeUniquePointer<lvk::VulkanContext>(contextConfig, window.GetNativeWindow());

    lvk::HWDeviceDesc devices[16];
    const uint32_t numDevices = context->queryDevices(devices, LVK_ARRAY_NUM_ELEMENTS(devices));

    if (!numDevices) {
        LVK_ASSERT_MSG(false, "GPU is not found");
    }
    int selectedDevice = -1;
    lvk::HWDeviceType preferredDeviceType = lvk::HWDeviceType_Discrete;
    
    if (selectedDevice < 0) {
        selectedDevice = [preferredDeviceType, &devices, numDevices]() -> int {
            // define device type priority order
            lvk::HWDeviceType priority[4] = {preferredDeviceType};
            {
                int index = 1;
                for (int type = lvk::HWDeviceType_Integrated; type <= lvk::HWDeviceType_Software; type++) {
                    if (type != preferredDeviceType)
                        priority[index++] = (lvk::HWDeviceType)type;
                }
            }
            // search devices in priority order
            for (lvk::HWDeviceType type : priority) {
                for (uint32_t i = 0; i < numDevices; i++) {
                    if (devices[i].type == type)
                        return (int)i;
                }
            }
            return 0;
        }();
    }

    if (selectedDevice >= numDevices) {
        LVK_ASSERT_MSG(false, "Invalid device index");
    }

    lvk::Result res = context->initContext(devices[selectedDevice]);

    if (!res.isOk()) {
        LVK_ASSERT_MSG(false, "createVulkanContextWithSwapchain() failed");
    }

    if (window.GetWindowSize().x > 0 && window.GetWindowSize().y > 0) {
        res = context->initSwapchain(window.GetWindowSize().x, window.GetWindowSize().y);
        if (!res.isOk()) {
            LVK_ASSERT_MSG(false, "initSwapchain() failed");
        }
    }
    
    auto imgui_ = std::make_unique<lvk::ImGuiRenderer>(
    *context, nullptr);

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
        pdlMaybeUnused float deltaTime = frameTimer.Lap<float, pdl::TimeTypes::Seconds>();
        perfWidget.Update(deltaTime, 0);
        auto inputManagerResults = inputManager.Update();
        window.Update();
        
        if (!inputManagerResults)
        {
            pdlLogError("%s", inputManagerResults.error().c_str());
        }

        lvk::RenderPass rp
        {
            
        };
        
        auto& cmd = context->acquireCommandBuffer();
        lvk::Framebuffer framebuffer{
                .color = {{.texture = context->getCurrentSwapchainTexture()}}
        };
        cmd.cmdBeginRendering({.color = {{.loadOp = lvk::LoadOp_Clear, .clearColor = {{1.0f, 0.0f, 1.0f, 0.0f}}}}},
                                 framebuffer);
        
        cmd.cmdBindRenderPipeline(renderPipelineState_Triangle_);
        cmd.cmdDraw(3);

        
        imgui_->beginFrame(framebuffer);
        perfWidget.ImGuiRender();
        imgui_->endFrame(cmd);
        cmd.cmdEndRendering();
        context->submit(cmd, context->getCurrentSwapchainTexture());

        
        // auto getCommandBufferResults = renderer->GetCommandBuffer();
        // if (!getCommandBufferResults)
        // {
        //     pdlLogError("%s", getCommandBufferResults.error().data());
        //     continue;
        // }
        // auto* commandBuffer = getCommandBufferResults.value();
        // pdl::RenderPass rp;
        // rp.m_colorAttachments[0].clearColor.x = 1.0f;
        // rp.m_colorAttachments[0].loadOp = pdl::LoadOp::Clear;
        // pdl::Framebuffer fb;
        // fb.m_colorAttachments[0].m_texture = renderer->GetCurrentSwapchainTexture();
        // commandBuffer->BeginRendering(rp, fb, {});
        // commandBuffer->EndRendering();
        // auto submitResults = renderer->SubmitCommandBuffer(commandBuffer, renderer->GetCurrentSwapchainTexture());
        // if (!submitResults)
        // {
        //     pdlLogError("Error submitting command buffer: %s", submitResults.error().data());
        //     continue;
        // }
        
        pdlLogFlush();
    }
    
    return 0;
    
}
