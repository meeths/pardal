
#include "Application/ApplicationWindow.h"
#include "Base/DebugHelpers.h"
#include "Log/Log.h"
#include "Log/LoggerStdout.h"
#include "Renderer/ISurface.h"
#include "Renderer/ITexture.h"
#include "Renderer/Renderer.h"
#include "Renderer/RenderPass.h"
#include "String/StringUtils.h"
#include "Time/Chronometer.h"


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
        .m_applicationWindow = window,
        .m_enableValidation = true
    });

    auto renderDevice = renderer.GetRenderDevice();

    const auto& deviceInfo = renderDevice->GetRenderDeviceInfo();
    pdlLogInfo("Render Device name: %s", deviceInfo.name.c_str());
    pdlLogInfo("Adapter: %s", deviceInfo.adapterName.c_str());
    pdlLogFlush();

    bool useHDR = true;

    auto surface = renderDevice->CreateSurface(window);
    if(!surface)
    {
        pdlLogError("Could not create surface");
        return -1;
    }

    
    pdl::ISurface::SwapchainDescriptor surfaceDescriptor
    {
        .m_format = useHDR ? pdl::Format::R16G16B16A16_FLOAT : pdl::Format::R8G8B8A8_UNORM,     
        .m_size= windowInitInfo.m_windowSize,
        .m_vsync= true
    };
    
    if (!(*surface)->ConfigureSwapchain(surfaceDescriptor))
    {
        pdlLogError("Could not configure swapchain");
        return -1;
    }
    
    pdl::SharedPointer<pdl::ITexture> depthBuffer;
    pdl::SharedPointer<pdl::ITextureView> depthBufferView;

    auto buildDepthBuffer = [&depthBuffer, &depthBufferView, &renderer](pdl::Math::Vector2i textureSize)
    {
        // Shared depth buffer
        pdl::ITexture::TextureDescriptor depthBufferDesc;
        depthBufferDesc.m_format = pdl::Format::D32_FLOAT;
        depthBufferDesc.m_extents.x = textureSize.x;
        depthBufferDesc.m_extents.y = textureSize.y;
        depthBufferDesc.m_extents.z = 1;
        depthBufferDesc.m_textureUsage = pdl::TextureUsage::DepthRead | pdl::TextureUsage::DepthWrite | pdl::TextureUsage::ShaderResource;

        auto depthBufferResult = renderer.GetRenderDevice()->CreateTexture(depthBufferDesc); 
        pdlAssert(depthBufferResult.has_value());
        depthBuffer = depthBufferResult.value();
    
        pdl::ITextureView::TextureViewDescriptor depthStencilViewDesc;
        depthStencilViewDesc.m_texture = depthBuffer.get();
        auto depthStencilViewResults = renderer.GetRenderDevice()->CreateTextureView(depthStencilViewDesc);
        pdlAssert(depthStencilViewResults.has_value());
        depthBufferView = depthStencilViewResults.value();
    };

    buildDepthBuffer(window.GetWindowSize());
    
    float maxColorComponentValue = useHDR ? 16.0f : 1.0f;
    float colorSpeed = useHDR ? 1.0f : 1/16.0f;

    window.AddResizeCallback([surface = (*surface), buildDepthBuffer](pdl::Math::Vector2i newSize)
    {
        buildDepthBuffer(newSize);
        auto swapchainDetails = surface->GetSurfaceConfig();
        swapchainDetails.m_size = newSize;
        surface->ConfigureSwapchain(swapchainDetails);
    });
    
    uint32 frameIndex = 0;
    pdl::Chronometer frameTimer;
    frameTimer.Start();
    while (!window.IsCloseRequested())
    {
        (*surface)->BeginFrame();
        pdl::Vector<pdl::ITextureView*> currentFrameSwapchainImageViews;
        pdl::Vector<pdl::Math::Vector4> clearColors;
        currentFrameSwapchainImageViews.push_back((*surface)->GetCurrentTextureView());
        clearColors.emplace_back(fmod(0.01f * frameIndex  * colorSpeed, maxColorComponentValue) , fmod(0.02f * frameIndex * colorSpeed , maxColorComponentValue) + 0.3, fmod(0.005f * frameIndex * colorSpeed , maxColorComponentValue) + 0.6, 1.0f);
        renderer.BeginFrame();
        pdl::RenderPass mainRenderPass(currentFrameSwapchainImageViews, depthBufferView.get(), pdl::Math::Rectanglei({0,0}, (*surface)->GetSurfaceConfig().m_size), clearColors);
        renderer.BeginRenderPass(mainRenderPass);
        renderer.EndRenderPass();
        renderer.EndFrame();
        (*surface)->Present();
        window.Update();
        ++frameIndex;

        window.SetWindowTitle(pdl::StringUtils::StringFormat("pdl test app: %.02f FPS", 1.0f/frameTimer.Lap<float, pdl::TimeTypes::Seconds>()));

        pdlLogFlush();
    }
    
    return 0;
    
}
