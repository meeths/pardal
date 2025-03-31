
#include <Renderer/Renderer.h>
#include <Log/Log.h>

#include "Application/ApplicationWindow.h"
#include "Vulkan/VulkanInternalRenderer.h"
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
        
        return true;
    }

    bool Renderer::BeginFrame()
    {
        return m_internalRenderer->BeginFrame();
    }

    bool Renderer::EndFrame()
    {
        return m_internalRenderer->EndFrame();
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
        return m_internalRenderer->EndRenderPass();
    }
}

