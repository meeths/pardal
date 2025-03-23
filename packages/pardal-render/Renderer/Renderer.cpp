
#include <Renderer/Renderer.h>
#include <Log/Log.h>
#ifdef PDL_VULKAN
#include <Renderer/Vulkan/VulkanDevice.h>
#endif
// Created on 2025-03-23 by sisco

namespace pdl
{
    bool Renderer::InitializeRenderDevice(IRenderDevice::InitInfoBase initInfo)
    {
        switch (initInfo.m_deviceType)
        {
#ifdef PDL_VULKAN
        case RenderDeviceType::Vulkan:
            m_device = MakeSharedPointer<VulkanDevice>();
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

        return true;
    }

}

