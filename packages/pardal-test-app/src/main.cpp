
#include "Application/ApplicationWindow.h"
#include "Base/DebugHelpers.h"
#include "Log/Log.h"
#include "Log/LoggerStdout.h"
#include "Renderer/ISurface.h"
#include "Renderer/ITexture.h"
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

    pdl::ISurface::SwapchainDescriptor surfaceDescriptor
    {
        .m_format= pdl::Format::R8G8B8A8_UNORM,
        .m_size= windowInitInfo.m_windowSize,
        .m_vsync= true
    };
    
    if (!(*surface)->ConfigureSwapchain(surfaceDescriptor))
    {
        pdlLogError("Could not configure swapchain");
        return -1;
    }

    constexpr uint32_t frameBufferCount = 2;
    pdl::Vector<pdl::SharedPointer<pdl::ITexture>> frameBuffers;
    
    for (uint32_t i = 0; i < frameBufferCount; ++i)
    {
        pdl::ITexture::TextureDescriptor depthBufferDesc;
        depthBufferDesc.m_format = pdl::Format::D32_FLOAT;
        depthBufferDesc.m_extents.x = windowInitInfo.m_windowSize.x;
        depthBufferDesc.m_extents.y = windowInitInfo.m_windowSize.y;
        depthBufferDesc.m_extents.z = 1;

        auto depthBuffer = renderer.GetRenderDevice()->CreateTexture(depthBufferDesc); 
        pdlAssert(depthBuffer.has_value());
        frameBuffers.push_back(depthBuffer.value());
    }
    
    while (!window.IsCloseRequested())
    {
        window.Update();
    }
    
    return 0;
    
}
