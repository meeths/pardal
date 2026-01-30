
#include "Renderer/Vulkan/VulkanSwapchain.h"

#include "VulkanTexture.h"
#include "Renderer/Vulkan/VulkanUtils.h"
#include "Renderer/Vulkan/VulkanRenderer.h"

// Created on 2026-01-30 by sisco

namespace Details
{
    vk::SurfaceFormatKHR GetSurfaceFormat(std::vector<vk::SurfaceFormatKHR>& availableFormats, pdl::ColorSpace colorSpace, bool hasColorSpaceExtension)
    {
        pdlAssert(!availableFormats.empty());
        
        const bool isBGR = [&availableFormats]()
        {
            for (const auto& formats : availableFormats)
            {
                if (formats.format == vk::Format::eR8G8B8A8Unorm ||
                    formats.format == vk::Format::eR8G8B8A8Srgb ||
                    formats.format == vk::Format::eA2R10G10B10UnormPack32)
                    return false;
                if (formats.format == vk::Format::eB8G8R8A8Unorm ||
                    formats.format == vk::Format::eB8G8R8A8Srgb ||
                    formats.format == vk::Format::eA2B10G10R10UnormPack32)
                    return true;
            }
            return false;
        }();
        
        const vk::SurfaceFormatKHR preferredFormat = [colorSpace, isBGR, hasColorSpaceExtension]()
        {
            switch (colorSpace)
            {
            case pdl::ColorSpace::Srgb:
                return vk::SurfaceFormatKHR{isBGR ? vk::Format::eB8G8R8A8Unorm : vk::Format::eR8G8B8A8Unorm, vk::ColorSpaceKHR::eSrgbNonlinear};
            case pdl::ColorSpace::Linear:
                if (hasColorSpaceExtension)
                    return vk::SurfaceFormatKHR{vk::Format::eR16G16B16A16Sfloat, vk::ColorSpaceKHR::eExtendedSrgbLinearEXT};
            case pdl::ColorSpace::HDR10:
                if (hasColorSpaceExtension)
                    return vk::SurfaceFormatKHR{isBGR ? vk::Format::eA2B10G10R10UnormPack32 : vk::Format::eA2R10G10B10UnormPack32, vk::ColorSpaceKHR::eHdr10St2084EXT};
            case pdl::ColorSpace::BT709:
                break;
            }
            return vk::SurfaceFormatKHR{ isBGR ? vk::Format::eB8G8R8A8Srgb : vk::Format::eR8G8B8A8Srgb, vk::ColorSpaceKHR::eSrgbNonlinear };
        }();
        
        for (const auto& formats : availableFormats)
        {
            if (formats.format == preferredFormat.format && formats.colorSpace == preferredFormat.colorSpace)
                return preferredFormat;
        }
        return availableFormats[0];
    }
}

namespace pdl
{
    VulkanSwapchain::VulkanSwapchain(VulkanRenderer& renderer, uint32 width, uint32 height, ColorSpace colorSpace) :
        m_renderer(renderer)
    {
        const auto& physicalDevice = m_renderer.GetPhysicalDevice();
        auto device = m_renderer.GetDevice();
        
        auto getSurfaceFormatsResults = physicalDevice.getSurfaceFormatsKHR(renderer.GetSurface());
        CHECK_VK_RESULTVALUE(getSurfaceFormatsResults);
        auto surfaceFormats = getSurfaceFormatsResults.value;
        
        m_swapchainFormat = Details::GetSurfaceFormat(surfaceFormats, colorSpace, true);
        
        auto queueFamilySupportsPresentation = physicalDevice.getSurfaceSupportKHR(renderer.GetDeviceQueues().graphicsQueueFamilyIndex, renderer.GetSurface());
        CHECK_VK_RESULTVALUE(queueFamilySupportsPresentation);
        pdlAssert(queueFamilySupportsPresentation.value == VK_TRUE && "The queue family used with the swapchain does not support presentation");

        auto surfaceCapabilitiesResults = physicalDevice.getSurfaceCapabilitiesKHR(renderer.GetSurface());
        CHECK_VK_RESULTVALUE(surfaceCapabilitiesResults);
        auto surfaceCapabilities = surfaceCapabilitiesResults.value;
        
        auto desiredImageCount = surfaceCapabilities.minImageCount + 1;
        desiredImageCount = desiredImageCount > surfaceCapabilities.maxImageCount ? surfaceCapabilities.maxImageCount : desiredImageCount;
   
        auto getPresentModesResults = physicalDevice.getSurfacePresentModesKHR(renderer.GetSurface());
        CHECK_VK_RESULTVALUE(getPresentModesResults);
        auto presentModes = getPresentModesResults.value;
        
        vk::PresentModeKHR presentMode = vk::PresentModeKHR::eFifo;
        if (std::ranges::find(presentModes, vk::PresentModeKHR::eMailbox) != presentModes.end())
            presentMode = vk::PresentModeKHR::eMailbox;
        else if (std::ranges::find(presentModes, vk::PresentModeKHR::eImmediate) != presentModes.end())
            presentMode = vk::PresentModeKHR::eImmediate;
        
        m_swapchainExtent.width = std::clamp(width, surfaceCapabilities.minImageExtent.width, surfaceCapabilities.maxImageExtent.width);
        m_swapchainExtent.height = std::clamp(height, surfaceCapabilities.minImageExtent.height, surfaceCapabilities.maxImageExtent.height);
        
        vk::FormatProperties2 formatProperties{};
        physicalDevice.getFormatProperties2(m_swapchainFormat.format, &formatProperties);
        
        vk::ImageUsageFlags imageUsageFlags = vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eTransferSrc;
        if (surfaceCapabilities.supportedUsageFlags & vk::ImageUsageFlagBits::eStorage &&
            (formatProperties.formatProperties.optimalTilingFeatures & vk::FormatFeatureFlagBits::eStorageImage))
            imageUsageFlags |= vk::ImageUsageFlagBits::eStorage;
            
        vk::SwapchainCreateInfoKHR swapchainCreateInfo = {};
        swapchainCreateInfo.surface = renderer.GetSurface();
        swapchainCreateInfo.minImageCount = desiredImageCount;
        swapchainCreateInfo.imageFormat = m_swapchainFormat.format;
        swapchainCreateInfo.imageColorSpace = m_swapchainFormat.colorSpace;
        swapchainCreateInfo.imageExtent = m_swapchainExtent;
        swapchainCreateInfo.imageArrayLayers = 1;
        swapchainCreateInfo.imageUsage = imageUsageFlags;
        swapchainCreateInfo.preTransform = surfaceCapabilities.currentTransform;
        swapchainCreateInfo.compositeAlpha = (surfaceCapabilities.supportedCompositeAlpha & vk::CompositeAlphaFlagBitsKHR::eOpaque) ? 
            vk::CompositeAlphaFlagBitsKHR::eOpaque : vk::CompositeAlphaFlagBitsKHR::eInherit;
        swapchainCreateInfo.presentMode = presentMode;
        swapchainCreateInfo.clipped = VK_TRUE;
        swapchainCreateInfo.oldSwapchain = VK_NULL_HANDLE;
        
        auto createSwapchainResults = device.createSwapchainKHR(swapchainCreateInfo);
        CHECK_VK_RESULTVALUE(createSwapchainResults);
        m_swapchain = createSwapchainResults.value;
        
        vk::HdrMetadataEXT hdrMetadata(
            {0.708f, 0.292f},
            {0.170f, 0.797f},
            {0.131f, 0.046f},
            {0.3127f, 0.3290f},
            80.0f,
            0.1f,
            2000.0f,
            400.0f
        );

        device.setHdrMetadataEXT(1, &m_swapchain, &hdrMetadata);

        auto getSwapchainImagesResults = device.getSwapchainImagesKHR(m_swapchain);
        CHECK_VK_RESULTVALUE(getSwapchainImagesResults);
        auto swapchainImages = getSwapchainImagesResults.value;
        m_swapchainImageCount = swapchainImages.size();
        pdlAssert(m_swapchainImageCount > 0 && m_swapchainImageCount <= kMaxSwapchainImages && "Invalid swapchain image count");

        for (uint32 i = 0; i < m_swapchainImageCount; ++i)
        {
            auto createSemaphoreResults = device.createSemaphore({});
            CHECK_VK_RESULTVALUE(createSemaphoreResults);
            m_acquireSemaphore[i] = createSemaphoreResults.value;

            auto createFenceResults = device.createFence({vk::FenceCreateFlagBits::eSignaled});
            CHECK_VK_RESULTVALUE(createFenceResults);
            m_acquireFence[i] = createFenceResults.value;
            
            {
                VulkanTexture image;
               image.m_vkImage = swapchainImages[i];
               image.m_vmaAllocation = nullptr;
               image.m_vkUsage = imageUsageFlags;
               image.m_vkFormat = m_swapchainFormat.format;
               image.m_vkExtent = vk::Extent3D{m_swapchainExtent.width, m_swapchainExtent.height, 1};
               image.m_vkSamples = vk::SampleCountFlagBits::e1;
               image.m_vkLayout = vk::ImageLayout::eUndefined;
            
               auto createImageResults = renderer.CreateTexture(image);
               if (!createImageResults)
               {
                   pdlLogError("%s", createImageResults.error().data());
                   pdlAssert(false);
               }
               m_swapchainImages[i] = createImageResults.value();
            }
            
            auto& image = *renderer.Get(m_swapchainImages[i]); 
            image.CreateDefaultView(renderer, vk::ImageViewType::e2D, m_swapchainFormat.format, vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1);
        }
    }

    VulkanSwapchain::~VulkanSwapchain()
    {
        for (auto& image : m_swapchainImages)
        {
            if (image)
            {
                m_renderer.Destroy(image);
            }
        }
        
        m_renderer.GetDevice().destroySwapchainKHR(m_swapchain);
    }

    Expected<void, StringView> VulkanSwapchain::Present(vk::Semaphore semaphore)
    {
        return {};
    }
}

