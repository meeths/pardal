#include <Renderer/Vulkan/VulkanRenderer.h>

#include "Application/ApplicationWindow.h"
#include "Base/ServiceLocator.h"
#include "ImGui/ImGuiRenderer.h"
#include "Log/Log.h"
#include "Renderer/Vulkan/lvk/vulkan/VulkanClasses.h"

namespace pdl
{
    VulkanRenderer::VulkanRenderer(const InitInfo& initInfo)
    {
        lvk::ContextConfig contextConfig;
        contextConfig.enableValidation = initInfo.m_enableValidation;
        contextConfig.terminateOnValidationError = false;
        contextConfig.vulkanVersion = lvk::VulkanVersion_1_3;
        contextConfig.swapchainRequestedColorSpace = initInfo.m_useHDR
                                                         ? lvk::ColorSpace_HDR10
                                                         : initInfo.m_useLinearColorSpace
                                                         ? lvk::ColorSpace_SRGB_EXTENDED_LINEAR
                                                         : lvk::ColorSpace_SRGB_NONLINEAR;
        
        m_context = MakeUniquePointer<lvk::VulkanContext>(contextConfig, initInfo.m_applicationWindow.GetNativeWindow());
        if (!m_context)
        {
            pdlLogError("Could not create Vulkan context");
            return;
        }
        auto initSwapchainResults = InitializeInstanceAndDevice(initInfo);
        if (!initSwapchainResults)
        {
            pdlLogError("%s", initSwapchainResults.error().data());
            return;
        }
        
#ifdef PDL_FEATURE_IMGUI
        ImGuiRenderer::InitInfo imGuiInitInfo { 
            .m_window = initInfo.m_applicationWindow,
            .m_renderer = this 
        };
        ServiceLocator<ImGuiRenderer>::Create();
        ServiceLocator<ImGuiRenderer>::Ref().Initialize(imGuiInitInfo);
#endif
    }

    VulkanRenderer::~VulkanRenderer()
    {
#ifdef PDL_FEATURE_IMGUI
        ServiceLocator<ImGuiRenderer>::Destroy();
#endif
    }

    Expected<void, StringView> VulkanRenderer::InitSwapchain(uint32 width, uint32 height)
    {
        if (width > 0 && height > 0)
        {
            auto res = m_context->initSwapchain(width, height);
            if (!res.isOk())
            {
                return Unexpected<StringView>("Failed to initialize swapchain");
            }
            return {};
        }
        return Unexpected<StringView>("Incorrect swapchain recreation dimensions");
    }

    lvk::IContext* VulkanRenderer::GetLVKContext() const
    {
        return m_context.get();
    }

    Expected<void, StringView> VulkanRenderer::InitializeInstanceAndDevice(
        const InitInfo& initInfo)
    {
        lvk::HWDeviceDesc devices[16];
        const uint32_t numDevices = m_context->queryDevices(devices, std::size(devices));

        if (!numDevices)
        {
            return Unexpected<StringView>("No suitable devices found");
        }
        
        int selectedDevice = initInfo.m_preferredDeviceIndex;
        lvk::HWDeviceType preferredDeviceType = lvk::HWDeviceType_Discrete;

        if (selectedDevice < 0)
        {
            selectedDevice = [&devices, numDevices, preferredDeviceType]() -> int
            {
                // define device type priority order
                lvk::HWDeviceType priority[4] = {preferredDeviceType};
                {
                    int index = 1;
                    for (int type = lvk::HWDeviceType_Integrated; type <= lvk::HWDeviceType_Software; type++)
                    {
                        if (type != preferredDeviceType)
                            priority[index++] = static_cast<lvk::HWDeviceType>(type);
                    }
                }
                // search devices in priority order
                for (lvk::HWDeviceType type : priority)
                {
                    for (uint32_t i = 0; i < numDevices; i++)
                    {
                        if (devices[i].type == type)
                            return static_cast<int>(i);
                    }
                }
                return 0;
            }();
        }

        if (selectedDevice >= static_cast<int>(numDevices))
        {
            return Unexpected<StringView>("Unexpected device index");
        }

        lvk::Result res = m_context->initContext(devices[selectedDevice]);

        if (!res.isOk())
        {
            return Unexpected<StringView>("Failed to initialize context");
        }

        auto windowRect = initInfo.m_applicationWindow.GetWindowSize();
        auto initSwapchainResults = InitSwapchain(windowRect.x, windowRect.y);
        if (!initSwapchainResults)
        {
            return Unexpected(initSwapchainResults.error());
        }

        initInfo.m_applicationWindow.AddResizeCallback([this](Math::Vector2 size)
        {
            GetLVKContext()->wait({});
            auto initSwapchainResults = InitSwapchain(static_cast<int>(size.x), static_cast<int>(size.y));
            if (!initSwapchainResults)
            {
                pdlLogError("%s", initSwapchainResults.error().data());
            }
        });

        return {};
    }
}
