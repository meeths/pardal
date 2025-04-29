
#include <Renderer/Renderer.h>
#include <Log/Log.h>
#include <Application/ApplicationWindow.h>
#include <Renderer/Vulkan/VulkanInternalRenderer.h>

#include "Base/DebugHelpers.h"
#include "Math/Vector3.h"
#ifdef PDL_VULKAN
#include <Renderer/Vulkan/VulkanDevice.h>
#endif
// Created on 2025-03-23 by sisco

namespace pdl
{
    class RenderPass;

    bool Renderer::InitializeRenderDevice(const IRenderDevice::InitInfoBase& initInfo)
    {
        switch (initInfo.m_deviceType)
        {
#ifdef PDL_VULKAN
        case RenderDeviceType::Vulkan:
            {
                auto vulkanDevice = MakeSharedPointer<VulkanDevice>(); 
                m_internalRenderer = MakeSharedPointer<VulkanInternalRenderer>(*vulkanDevice.get());
                m_device = vulkanDevice;
            }
            break;
#endif            
        default: ;
            pdlLogError("Renderer::InitializeRenderDevice: Unknown device type <%s>", to_string(initInfo.m_deviceType));
            return false;
            break;
        }

        if(!m_device->Initialize(initInfo))
        {
            return false;
        }

        IInternalRenderer::InitInfo internalRendererInitInfo
        {
            .m_window = initInfo.m_applicationWindow,
            .m_initialSurfaceSize = initInfo.m_applicationWindow.GetWindowSize(),
            .m_useVSync = initInfo.m_useVSync,
            .m_useHDR = initInfo.m_useHDR,
            .m_createDepthBuffer = true,
        };
        m_internalRenderer->Initialize(internalRendererInitInfo);
        initInfo.m_applicationWindow.AddResizeCallback([internalRenderer = m_internalRenderer.get()](Math::Vector2i newSize)
        {
            internalRenderer->OnResize(newSize);
        });

        ImGuiRenderer::InitInfo imguiInitInfo
        {
            .m_window = initInfo.m_applicationWindow,
            .m_device = m_device.get(),
            .m_useHDR = initInfo.m_useHDR
        };
        
        m_imguiRenderer = MakeSharedPointer<ImGuiRenderer>(imguiInitInfo);

        m_frameInfo.eyePos = Math::Vector3(0.0f, 0.0f, 0.0f);
        m_frameInfo.projection = GetRenderDevice()->GetRenderDeviceInfo().identityProjection;
        m_frameInfo.view = glm::identity<Math::Matrix44>();

        pdl::IRenderBuffer::BufferDescriptor frameInfoBufferDescriptor;
        frameInfoBufferDescriptor.size = sizeof(PerFrameInfo);
        frameInfoBufferDescriptor.usage = pdl::BufferUsage::ShaderResource;
        frameInfoBufferDescriptor.memoryType = pdl::MemoryType::Upload;

        auto frameInfoBuffer = GetRenderDevice()->CreateRenderBuffer(frameInfoBufferDescriptor);
        if (!frameInfoBuffer)
        {
            pdlLogError("Failed to create frame info buffer: %s", frameInfoBuffer.error().data());
            return false;
        }
        m_frameInfoBuffer = frameInfoBuffer.value();
        UpdateFrameInfoBuffer();

        return true;
    }

    Renderer::~Renderer()
    {
    }

    void Renderer::Update(float deltaTime)
    {
        m_frameInfo.deltaTime = deltaTime;
        m_frameInfo.totalTime += deltaTime;
        ++m_frameInfo.frameIndex;
    }

    bool Renderer::BeginFrame()
    {
        UpdateFrameInfoBuffer();
        m_imguiRenderer->BeginFrame();
        return m_internalRenderer->BeginFrame();
        
    }

    bool Renderer::EndFrame()
    {
        bool frameEnded = m_internalRenderer->EndFrame();
        m_imguiRenderer->EndFrame();
        return frameEnded;
    }

    RenderPass& Renderer::GetMainRenderPass()
    {
        return m_internalRenderer->GetMainRenderPass();
    }

    bool Renderer::BeginRenderPass(const RenderPass& renderPass)
    {
        return m_internalRenderer->BeginRenderPass(renderPass);
    }
    
    bool Renderer::EndRenderPass()
    {
        m_imguiRenderer->Render();
        return m_internalRenderer->EndRenderPass();
    }

    void Renderer::UpdateFrameInfoBuffer()
    {
        pdlAssert(m_frameInfoBuffer);
        PerFrameInfo* frameInfo = reinterpret_cast<PerFrameInfo*>(m_frameInfoBuffer->Map());
        memcpy(frameInfo, &m_frameInfo, sizeof(PerFrameInfo));
        m_frameInfoBuffer->Unmap();
    }
}

