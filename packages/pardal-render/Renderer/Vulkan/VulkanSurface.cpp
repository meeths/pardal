
#include <Renderer/Vulkan/VulkanSurface.h>
#include <Renderer/Vulkan/VulkanUtils.h>
#include <Application/ApplicationWindow.h>
#include <Renderer/Vulkan/VulkanTexture.h>

// Created on 2025-03-23 by sisco

namespace pdl
{
    VulkanSurface::~VulkanSurface()
    {
        DestroySwapchain();
        m_vkInstance->destroySurfaceKHR(m_vkSurface);
        m_vkDevice->destroySemaphore(m_vkNextImageAcquireSemaphore);
    }

    bool VulkanSurface::Initialize(vk::Device* device, vk::PhysicalDevice* physicalDevice, vk::Instance* instance, ApplicationWindow& windowHandle, Format preferredFormat)
    {
        m_vkDevice = device;
        m_vkPhysicalDevice = physicalDevice;
        m_vkInstance = instance;

        vk::SemaphoreCreateInfo semaphoreCreateInfo = {};
        auto createSemaphoreResult = m_vkDevice->createSemaphore(semaphoreCreateInfo);
        CHECK_VK_RESULTVALUE(createSemaphoreResult);
        m_vkNextImageAcquireSemaphore = createSemaphoreResult.value;

#ifdef PDL_PLATFORM_WINDOWS
        vk::Win32SurfaceCreateInfoKHR surfaceCreateInfo = {};
        surfaceCreateInfo.hinstance = static_cast<HINSTANCE>(windowHandle.GetNativeModuleHandle());
        surfaceCreateInfo.hwnd = static_cast<HWND>(windowHandle.GetNativeWindow());
        auto surfaceCreateResults = instance->createWin32SurfaceKHR(surfaceCreateInfo);
        CHECK_VK_RESULTVALUE(surfaceCreateResults);
        m_vkSurface = surfaceCreateResults.value;
       
#else
#error Implement relevant surface creation for this platform
#endif
        auto surfaceFormatsResult = physicalDevice->getSurfaceFormatsKHR(m_vkSurface);
        CHECK_VK_RESULTVALUE(surfaceFormatsResult);
        auto surfaceFormats = surfaceFormatsResult.value;

        for (const auto& vkFormat : surfaceFormats)
        {
            Format format = VulkanUtils::TranslateFromVkFormat(vkFormat.format);
            if (format != Format::Unknown)
                m_info.m_supportedFormats.push_back(format);
            if (format == preferredFormat)
                m_info.m_preferredFormat = format;
        }
        if (m_info.m_preferredFormat == Format::Unknown && !m_info.m_supportedFormats.empty())
        {
            m_info.m_preferredFormat = m_info.m_supportedFormats.front();
        }
        m_info.m_supportedUsage = TextureUsage::Present | TextureUsage::RenderTarget | TextureUsage::CopyDestination;

        return true;
    }

    bool VulkanSurface::CreateSwapchain()
    {
        VkExtent2D imageExtent = {static_cast<uint32_t>(m_config.m_size.x), static_cast<uint32_t>(m_config.m_size.y)};

        auto surfaceCapsResult = m_vkPhysicalDevice->getSurfaceCapabilitiesKHR(m_vkSurface);
        CHECK_VK_RESULTVALUE(surfaceCapsResult);

        auto presentModesResult = m_vkPhysicalDevice->getSurfacePresentModesKHR(m_vkSurface);
        CHECK_VK_RESULTVALUE(presentModesResult);

        static const Vector<vk::PresentModeKHR> kVsyncOffModes {
            vk::PresentModeKHR::eImmediate,
            vk::PresentModeKHR::eMailbox,
            vk::PresentModeKHR::eFifo
        };
        static const Vector<vk::PresentModeKHR> kVsyncOnModes {
            vk::PresentModeKHR::eFifoRelaxed,
            vk::PresentModeKHR::eFifo,
            vk::PresentModeKHR::eImmediate,
            vk::PresentModeKHR::eMailbox
        };
        const Vector<vk::PresentModeKHR>& checkPresentModes = m_config.m_vsync ? kVsyncOnModes : kVsyncOffModes;
        vk::PresentModeKHR selectedPresentMode{};
        bool presentAvailable = false;
        for (auto checkPresentMode : checkPresentModes)
        {
            if (std::ranges::find(presentModesResult.value, checkPresentMode) != presentModesResult.value.end())
            {
                selectedPresentMode = checkPresentMode;
                presentAvailable = true;
                break;
            }
        }
        if (!presentAvailable)
        {
            pdlLogError("VulkanSurface::CreateSwapchain Present mode unavailable");
            return false;
        }

        vk::Format format = VulkanUtils::TranslateToVkFormat(m_config.m_format);
        vk::SwapchainCreateInfoKHR swapchainCreateInfo {
            {},
            m_vkSurface,
            m_config.m_desiredImageCount,
            format,
            vk::ColorSpaceKHR::eSrgbNonlinear,
            imageExtent,
            1,
            VulkanUtils::TranslateToVkImageUsageFlags(m_config.m_usage),
            vk::SharingMode::eExclusive,
            {},
            {},
            vk::SurfaceTransformFlagBitsKHR::eIdentity,
            vk::CompositeAlphaFlagBitsKHR::eOpaque,
            selectedPresentMode,
            true,
            {},            
        };

        auto createSwapchainResult = m_vkDevice->createSwapchainKHR(swapchainCreateInfo);
        CHECK_VK_RESULTVALUE(createSwapchainResult);
        m_vkSwapchain = createSwapchainResult.value;

        auto getSwapchainImagesResult = m_vkDevice->getSwapchainImagesKHR(m_vkSwapchain);
        CHECK_VK_RESULTVALUE(getSwapchainImagesResult);
        auto swapchainImages = getSwapchainImagesResult.value;

        for (auto& image : swapchainImages)
        {
            ITexture::TextureDescriptor desc;
            desc.m_arraySize = 0;
            desc.m_format = m_config.m_format;
            desc.m_extents.z = 1;
            desc.m_extents.y = m_config.m_size.y;
            desc.m_extents.x = m_config.m_size.x;
            desc.m_mipLevels = 1;
            
            VulkanTexture texture(desc, m_vkDevice);
            texture.Initialize(image);
            
            m_images.push_back(std::move(texture));
        }
        
        return true;
    }

    void VulkanSurface::DestroySwapchain()
    {
        m_vkDevice->destroySwapchainKHR(m_vkSwapchain);
    }

    bool VulkanSurface::ConfigureSwapchain(SwapchainDescriptor config)
    {
        m_config = config;
        if (m_config.m_size.x == 0|| m_config.m_size.y == 0)
        {
            pdlLogError("VulkanSurface::Configure: Invalid width/height");
            return false;
        }
        if (std::ranges::find(m_info.m_supportedFormats, m_config.m_format) == m_info.m_supportedFormats.end())
        {
            pdlLogWarning("VulkanSurface::Configure: unsupported format %s. Using default %s.", to_string(config.m_format), to_string(m_info.m_preferredFormat));
            m_config.m_format = m_info.m_preferredFormat;
        }
        DestroySwapchain();
        return CreateSwapchain();
    }

    ITexture* VulkanSurface::GetTexture()
    {
        pdlAssert(!m_images.empty() && "VulkanSurface::GetTexture: No images available");
        pdlAssert(m_images.size() > m_currentImageIndex && "VulkanSurface::GetTexture: Incorrect current image index");
        
        return &m_images[m_currentImageIndex];    }

    bool VulkanSurface::Present()
    {
        pdlAssert(!m_images.empty() && "VulkanSurface::Present: No images available to present");
        pdlNotImplemented();
        return false;
    }
}

