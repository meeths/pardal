
#include "Application/ApplicationWindow.h"
#include "Base/DebugHelpers.h"
#include "ImGui/ImGuiPerfWidget.h"
#include "Input/InputManager.h"
#include "Log/Log.h"
#include "Log/LoggerStdout.h"
#include "Math/Vector3.h"
#include "Memory/Memory.h"
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
    
    // Testing renderer objects creation
    auto createBufferResults = renderer->CreateBuffer(1024*1024, pdl::BufferUsage::ConstantBuffer, pdl::MemoryType::DeviceLocal);
    if (!createBufferResults)
    {
        pdlLogError("%s", createBufferResults.error().data());
    }
    auto renderBuffer = createBufferResults.value();
    renderer->Destroy(renderBuffer);
    renderer->Destroy(renderBuffer);    // Logs warning
    
    
    while (!window.IsCloseRequested())
    {
        pdlMaybeUnused float deltaTime = frameTimer.Lap<float, pdl::TimeTypes::Seconds>();
        
        auto inputManagerResults = inputManager.Update();
        window.Update();
        
        if (!inputManagerResults)
        {
            pdlLogError("%s", inputManagerResults.error().c_str());
        }
        
        pdlLogFlush();
    }
    
    return 0;
    
}
