
#include "Application/ApplicationWindow.h"
#include "Base/DebugHelpers.h"
#include "Log/Log.h"
#include "Log/LoggerStdout.h"
#include "Math/Vector3.h"
#include "Memory/Memory.h"
#include "Renderer/BindlessDescriptors.h"
#include "Renderer/ISurface.h"
#include "Renderer/ITexture.h"
#include "Renderer/Renderer.h"
#include "Renderer/RenderPass.h"
#include "Renderer/Shaders/device_host_structs.h"
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

    bool useHDR = false;

    pdl::Renderer renderer;
    if (!renderer.InitializeRenderDevice({
        .m_deviceType = pdl::RenderDeviceType::Vulkan,
        .m_applicationName = "pardal-test-app",
        .m_applicationWindow = window,
        .m_enableValidation = true,
        .m_useVSync = false,
        .m_useHDR = useHDR
    }))
    {
        return -1;
    }

   
    float maxColorComponentValue = useHDR ? 16.0f : 1.0f;
    float colorSpeed = useHDR ? 1.0f : 1/16.0f;

    pdl::Chronometer frameTimer;
    frameTimer.Start();

    pdl::ITexture::TextureDescriptor textureDescriptor;
    textureDescriptor.m_textureUsage = pdl::TextureUsage::ShaderResource | pdl::TextureUsage::CopyDestination;
    textureDescriptor.m_textureType = pdl::TextureType::Texture2D;
    textureDescriptor.m_extents = {512, 512, 1};
    textureDescriptor.m_format = pdl::Format::R8G8B8A8_UNORM;
    textureDescriptor.m_mipLevels = 1;
    textureDescriptor.m_arraySize = 1;
    auto newTexture = renderer.GetRenderDevice()->CreateTexture(textureDescriptor);
    if (!newTexture.has_value())
    {
        pdlLogError(newTexture.error().data());
    }
    pdl::SharedPointer<pdl::ITexture> texture = newTexture.value();
    
    pdl::ITextureView::TextureViewDescriptor textureViewDescriptor;
    textureViewDescriptor.m_texture = texture.get();
    textureViewDescriptor.m_baseMipLevel = 0;
    
    
    auto textureView = renderer.GetRenderDevice()->CreateTextureView(textureViewDescriptor);
    
    renderer.GetBindlessDescriptors()->StoreTexture(textureView->get());
    
    while (!window.IsCloseRequested())
    {
        float deltaTime = frameTimer.Lap<float, pdl::TimeTypes::Seconds>();
        renderer.Update(deltaTime);
        
        renderer.BeginFrame();
        auto mainRenderPass = renderer.GetMainRenderPass();
        uint32 frameIndex = renderer.GetFrameInfo().frameIndex;
        mainRenderPass.SetClearColor(0,
                                         pdl::Math::Vector4(fmod(0.01f * frameIndex * colorSpeed, maxColorComponentValue),
                                        fmod(0.02f * frameIndex * colorSpeed, maxColorComponentValue) + 0.3,
                                        fmod(0.005f * frameIndex * colorSpeed,
                                        maxColorComponentValue) + 0.6, 1.0f));
        
        pdl::Math::Rectangle windowSize = {{0,0}, {window.GetWindowSize().x, window.GetWindowSize().y}};
        pdl::Math::Rectanglei scissorSize = {{0,0}, {window.GetWindowSize().x, window.GetWindowSize().y}};
        renderer.SetViewport(windowSize);
        renderer.SetScissor(scissorSize);
        
        renderer.BeginRenderPass(mainRenderPass);
        
        renderer.EndRenderPass();
        renderer.EndFrame();
        window.Update();

        window.SetWindowTitle(pdl::StringUtils::StringFormat("pdl test app: %.02f FPS", 1.0f/frameTimer.Lap<float, pdl::TimeTypes::Seconds>()));

        pdlLogFlush();
    }
    
    return 0;
    
}
