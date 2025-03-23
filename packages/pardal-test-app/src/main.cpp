
#include "Application/ApplicationWindow.h"
#include "Log/Log.h"
#include "Log/LoggerStdout.h"
#include "Renderer/ISurface.h"
#include "Renderer/Renderer.h"

int main(int argc, char** argv)
{
#ifndef PDL_RELEASE
    pdl::Log::Instance().RegisterLogger(MakeSharedPointer<pdl::LoggerStdout>());
#endif

    pdl::IApplicationWindow::InitInfoBase windowInitInfo;
    windowInitInfo.m_windowTitle = "pardal-test-app";

    pdl::ApplicationWindow window(windowInitInfo);

    pdl::Renderer renderer;
    renderer.InitializeRenderDevice({
        .m_deviceType = pdl::RenderDeviceType::Vulkan,
        .m_applicationName = "pardal-test-app",
        .m_enableValidation = true
    });

    auto renderDevice = renderer.GetRenderDevice();

    const auto& deviceInfo = renderDevice->GetRenderDeviceInfo();
    pdlLogInfo("Render Device name: %s", deviceInfo.name.c_str());
    pdlLogInfo("Adapter: %s", deviceInfo.adapterName.c_str());
    pdlLogFlush();

    auto surface = renderDevice->CreateSurface(window);
    if(!surface)
    {
        pdlLogError("Could not create surface");
        return -1;
    }

    pdl::ISurface::Descriptor surfaceDescriptor
    {
        .m_format= pdl::Format::R8G8B8A8_UNORM,
        .m_size= windowInitInfo.m_windowSize,
        .m_vsync= true
    };
    
    if (!(*surface)->Configure(surfaceDescriptor))
    {
        pdlLogError("Could not configure swapchain");
        return -1;
    }
    
    while (!window.IsCloseRequested())
    {
        window.Update();
    }
    
    return 0;
    
}
