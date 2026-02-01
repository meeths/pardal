
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
#include "String/StringUtils.h"
#include "Time/Chronometer.h"


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

    pdl::Chronometer frameTimer;
    frameTimer.Start();

    pdl::InputManager inputManager;
    
    while (!window.IsCloseRequested())
    {
        pdlMaybeUnused float deltaTime = frameTimer.Lap<float, pdl::TimeTypes::Seconds>();
        
        auto inputManagerResults = inputManager.Update();
        window.Update();
        
        if (!inputManagerResults)
        {
            pdlLogError("%s", inputManagerResults.error().c_str());
        }

        auto getCommandBufferResults = renderer->GetCommandBuffer();
        if (!getCommandBufferResults)
        {
            pdlLogError("%s", getCommandBufferResults.error().data());
            continue;
        }
        auto* commandBuffer = getCommandBufferResults.value();
        pdl::RenderPass rp;
        rp.m_colorAttachments[0].clearColor.x = 1.0f;
        rp.m_colorAttachments[0].loadOp = pdl::LoadOp::Clear;
        pdl::Framebuffer fb;
        fb.m_colorAttachments[0].m_texture = renderer->GetCurrentSwapchainTexture();
        commandBuffer->BeginRendering(rp, fb, {});
        commandBuffer->EndRendering();
        auto submitResults = renderer->SubmitCommandBuffer(commandBuffer, renderer->GetCurrentSwapchainTexture());
        if (!submitResults)
        {
            pdlLogError("Error submitting command buffer: %s", submitResults.error().data());
            continue;
        }
        
        pdlLogFlush();
    }
    
    return 0;
    
}
