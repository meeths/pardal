
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

    bool useHDR = false;

    pdl::Renderer renderer;
    renderer.InitializeRenderDevice({
        .m_deviceType = pdl::RenderDeviceType::Vulkan,
        .m_applicationName = "pardal-test-app",
        .m_applicationWindow = window,
        .m_enableValidation = true,
        .m_useVSync = false,
        .m_useHDR = useHDR
    });

   
    float maxColorComponentValue = useHDR ? 16.0f : 1.0f;
    float colorSpeed = useHDR ? 1.0f : 1/16.0f;

    pdl::IRenderBuffer::BufferDescriptor coolBufferDescriptor;
    coolBufferDescriptor.size = 1024 * 1024 * 10;
    coolBufferDescriptor.usage = pdl::BufferUsage::VertexBuffer | pdl::BufferUsage::ShaderResource;
    coolBufferDescriptor.memoryType = pdl::MemoryType::Upload;
    
    auto coolBuffer = renderer.GetRenderDevice()->CreateRenderBuffer(coolBufferDescriptor);
    uint8* data = coolBuffer.value()->Map();
    for (uint32 i = 0; i < 1024 * 1024 * 10; i++)
    {
        data[i] = 128;
    }
    coolBuffer.value()->Unmap();
    
    uint32 frameIndex = 0;
    pdl::Chronometer frameTimer;
    frameTimer.Start();
    while (!window.IsCloseRequested())
    {
        renderer.BeginFrame();
        auto mainRenderPass = renderer.GetMainRenderPass();
        mainRenderPass.SetClearColor(0,
                                     pdl::Math::Vector4(fmod(0.01f * frameIndex * colorSpeed, maxColorComponentValue),
                                                        fmod(0.02f * frameIndex * colorSpeed, maxColorComponentValue) +
                                                        0.3, fmod(0.005f * frameIndex * colorSpeed,
                                                                  maxColorComponentValue) + 0.6, 1.0f));
        renderer.BeginRenderPass(mainRenderPass);
        renderer.EndRenderPass();
        renderer.EndFrame();
        window.Update();
        ++frameIndex;

        window.SetWindowTitle(pdl::StringUtils::StringFormat("pdl test app: %.02f FPS", 1.0f/frameTimer.Lap<float, pdl::TimeTypes::Seconds>()));

        pdlLogFlush();
    }
    
    return 0;
    
}
