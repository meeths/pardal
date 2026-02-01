#include <iostream>
#include <Renderer/Vulkan/VulkanRenderer.h>

#include "VkExtensionsFeaturesHelper.h"
#include "VulkanBuffer.h"
#include "VulkanCommandBuffer.h"
#include "VulkanUtils.h"
#include "Application/ApplicationWindow.h"
#include "Containers/VectorUtils.h"
#include "Log/Log.h"
#include "Math/Functions.h"
#include "Renderer/RenderererConstants.h"
#include "Renderer/RenderererDevices.h"
#include "Renderer/Vulkan/VulkanImmediateCommand.h"
#include "Renderer/Vulkan/VulkanSwapchain.h"
#include "String/StringUtils.h"

#ifdef PDL_PLATFORM_WINDOWS
#include <dxgi1_2.h>
#endif

#ifndef VULKAN_HPP_DISPATCH_LOADER_DYNAMIC
#error VULKAN_HPP_DISPATCH_LOADER_DYNAMIC is required
#endif

VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE

#define VMA_IMPLEMENTATION
#pragma clang diagnostic ignored "-Wunused-variable"
#include <vma/vk_mem_alloc.h>

// Created on 2026-01-26 by sisco
namespace Details
{
    VmaAllocator g_vmaAllocator = VK_NULL_HANDLE;
    
    static vk::PhysicalDevice& PickBestDevice(pdl::Vector<vk::PhysicalDevice>& devices)
    {
        pdlAssert(!devices.empty());
        uint32 bestDeviceIndex = 0;
        size_t mostVRAMfound = 0;

        // Find the best one if there's more than one
        if (devices.size() > 1)
        {
            for (uint32 deviceIndex = 0; deviceIndex < devices.size(); ++deviceIndex)
            {
                auto& device = devices.at(deviceIndex);
                auto deviceProperties = device.getProperties();

                // Always favor discrete gpus over anything else
                if ((deviceIndex == 0 || devices[bestDeviceIndex].getProperties().deviceType != vk::PhysicalDeviceType::eDiscreteGpu) &&
                    deviceProperties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu)
                {
                    bestDeviceIndex = deviceIndex;
                    continue;
                }

                if (devices[bestDeviceIndex].getProperties().deviceType == vk::PhysicalDeviceType::eDiscreteGpu &&
                    deviceProperties.deviceType != vk::PhysicalDeviceType::eDiscreteGpu)
                {
                    continue;
                }

                // Next best indicator is memory
                auto memoryProperties = device.getMemoryProperties();
                for (auto& heap : memoryProperties.memoryHeaps)
                {
                    if (heap.flags & vk::MemoryHeapFlagBits::eDeviceLocal)
                    {
                        if (heap.size > mostVRAMfound)
                        {
                            mostVRAMfound = heap.size;
                            bestDeviceIndex = deviceIndex;
                        }
                    }
                }
            }
        }

        return devices.at(bestDeviceIndex);
    }

    static VkBool32 DebugUtilsMessengerCallback(vk::DebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                vk::DebugUtilsMessageTypeFlagsEXT messageTypes,
                                                vk::DebugUtilsMessengerCallbackDataEXT const* pCallbackData,
                                                void* /*pUserData*/)
    {
        pdl::String debugMessage = pdl::StringUtils::StringFormat("[Vulkan %s] %s: %s [ID%s%d]",
                                                                vk::to_string(messageSeverity).c_str(),
                                                                vk::to_string(messageTypes).c_str(),
                                                                pCallbackData->pMessage,
                                                                pCallbackData->pMessageIdName,
                                                                pCallbackData->messageIdNumber
        );

        if (pCallbackData->queueLabelCount > 0)
        {
            debugMessage += "\n{Queues: ";
            for (uint32_t i = 0; i < pCallbackData->queueLabelCount; i++)
            {
                debugMessage += pCallbackData->pQueueLabels[i].pLabelName;
                debugMessage += " ";
            }
            debugMessage += "}";
        }

        if (pCallbackData->cmdBufLabelCount > 0)
        {
            debugMessage += "\n{CmdBufs: ";
            for (uint32_t i = 0; i < pCallbackData->cmdBufLabelCount; i++)
            {
                debugMessage += pCallbackData->pCmdBufLabels[i].pLabelName;
                debugMessage += " ";
            }
            debugMessage += "}";
        }

        if (pCallbackData->objectCount > 0)
        {
            debugMessage += "\n{Objects: ";
            for (uint32_t i = 0; i < pCallbackData->objectCount; i++)
            {
                debugMessage += pdl::StringUtils::StringFormat("%s %llx [%s],  ",
                                                                vk::to_string(
                                                                    static_cast<vk::ObjectType>(pCallbackData->pObjects[
                                                                        i].objectType)).c_str(),
                                                                pCallbackData->pObjects[i].objectHandle,
                                                                pCallbackData->pObjects[i].pObjectName
                                                                    ? pCallbackData->pObjects[i].pObjectName
                                                                    : "???");
            }
            debugMessage += "}";
        }


        if (messageSeverity & vk::DebugUtilsMessageSeverityFlagBitsEXT::eError)
        {
            pdlLogError("%s", debugMessage.c_str());
        }
        else if (messageSeverity & vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning)
        {
            pdlLogWarning("%s", debugMessage.c_str());
        }
        else
        {
            pdlLogInfo("%s", debugMessage.c_str());
        }

        return false;
    }

    static int FindQueue(const vk::PhysicalDevice& device, vk::QueueFlags queueFlags)
    {
        for (int i = 0; i < device.getQueueFamilyProperties().size(); i++)
        {
            if (device.getQueueFamilyProperties()[i].queueFlags & queueFlags)
            {
                return i;
            }
        }
        return -1;
    }

    pdl::Expected<VmaAllocator, pdl::StringView> CreateVmaAllocator(const vk::PhysicalDevice& physDev, const vk::Device& device, const vk::Instance instance, uint32_t apiVersion)
    {
        const VmaVulkanFunctions funcs = {
            .vkGetInstanceProcAddr = vkGetInstanceProcAddr,
            .vkGetDeviceProcAddr = vkGetDeviceProcAddr,
            .vkGetPhysicalDeviceProperties = vkGetPhysicalDeviceProperties,
            .vkGetPhysicalDeviceMemoryProperties = vkGetPhysicalDeviceMemoryProperties,
            .vkAllocateMemory = vkAllocateMemory,
            .vkFreeMemory = vkFreeMemory,
            .vkMapMemory = vkMapMemory,
            .vkUnmapMemory = vkUnmapMemory,
            .vkFlushMappedMemoryRanges = vkFlushMappedMemoryRanges,
            .vkInvalidateMappedMemoryRanges = vkInvalidateMappedMemoryRanges,
            .vkBindBufferMemory = vkBindBufferMemory,
            .vkBindImageMemory = vkBindImageMemory,
            .vkGetBufferMemoryRequirements = vkGetBufferMemoryRequirements,
            .vkGetImageMemoryRequirements = vkGetImageMemoryRequirements,
            .vkCreateBuffer = vkCreateBuffer,
            .vkDestroyBuffer = vkDestroyBuffer,
            .vkCreateImage = vkCreateImage,
            .vkDestroyImage = vkDestroyImage,
            .vkCmdCopyBuffer = vkCmdCopyBuffer,
            .vkGetBufferMemoryRequirements2KHR = vkGetBufferMemoryRequirements2,
            .vkGetImageMemoryRequirements2KHR = vkGetImageMemoryRequirements2,
            .vkBindBufferMemory2KHR = vkBindBufferMemory2,
            .vkBindImageMemory2KHR = vkBindImageMemory2,
            .vkGetPhysicalDeviceMemoryProperties2KHR = vkGetPhysicalDeviceMemoryProperties2,
            .vkGetDeviceBufferMemoryRequirements = vkGetDeviceBufferMemoryRequirements,
            .vkGetDeviceImageMemoryRequirements = vkGetDeviceImageMemoryRequirements,
        };

        const VmaAllocatorCreateInfo ci = {
            .flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT,
            .physicalDevice = physDev,
            .device = device,
            .preferredLargeHeapBlockSize = 0,
            .pAllocationCallbacks = nullptr,
            .pDeviceMemoryCallbacks = nullptr,
            .pHeapSizeLimit = nullptr,
            .pVulkanFunctions = &funcs,
            .instance = instance,
            .vulkanApiVersion = apiVersion,
        };
        VmaAllocator vma = VK_NULL_HANDLE;
        auto vmaCreateResults = vmaCreateAllocator(&ci, &vma);
        
        if (vmaCreateResults != VK_SUCCESS)
        {
            pdlLogError("VMA allocator creation failed with error code %d", vmaCreateResults);
            return pdl::Unexpected("Vulkan allocator initialization failed");
        }

        return vma;
    }
}

namespace pdl
{
    VulkanRenderer::VulkanRenderer(const InitInfo& initInfo)
    {
        m_deviceInfo.deviceType = RenderDeviceType::Vulkan;
        m_deviceInfo.name = "pdl::VulkanRenderer";
        m_deviceInfo.identityProjection = glm::identity<Math::Matrix44>();

        auto initResults = InitializeInstanceAndDevice(initInfo);

        if (!initResults)
        {
            pdlLogError("Vulkan device initialization failed:\n %s", initResults.error().data());
            return;
        }

        auto windowSize = initInfo.m_applicationWindow.GetWindowSize();
        auto createSwapchainResults = InitSwapchain(windowSize.x, windowSize.y);

#ifdef PDL_DEBUG
        pdlLogInfo("Render Device name: %s", m_deviceInfo.name.c_str());
        pdlLogInfo("Adapter: %s", m_deviceInfo.adapterName.c_str());
        pdlLogFlush();
#endif
    }

    VulkanRenderer::~VulkanRenderer()
    {
    }

    Expected<void, StringView> VulkanRenderer::InitSwapchain(uint32 width, uint32 height)
    {
        if (!m_vkDevice)
        {
            return Unexpected("Vulkan device not initialized");
        }
        
        // Destroy previous is this is about recreating
        if (m_swapchain)
        {
            auto waitResults = m_vkDevice.waitIdle();
            CHECK_VK_RESULT(waitResults);
            m_swapchain.reset();
            m_vkDevice.destroySemaphore(m_vkTimelineSemaphore);
        }
        
        m_swapchain = MakeUniquePointer<VulkanSwapchain>(*this, width, height, ColorSpace::Srgb);
        return {};
    }

    Expected<ICommandBuffer*, StringView> VulkanRenderer::GetCommandBuffer()
    {
        if (!m_vkDevice)
        {
            return Unexpected("Vulkan device not initialized");
        }
        return nullptr;
    }

    Expected<void, StringView> VulkanRenderer::SubmitCommandBuffer(ICommandBuffer* commandBuffer, TextureHandle presentTarget)
    {
        if (!m_vkDevice)
        {
            return Unexpected("Vulkan device not initialized");
        }
        
        VulkanCommandBuffer* vkCommandBuffer = static_cast<VulkanCommandBuffer*>(commandBuffer);
        pdlAssert(vkCommandBuffer);
        if (vkCommandBuffer->IsRecording())
        {
            return Unexpected("Cannot submit command buffer that is still recording");
        }

        return {};
    }

    Expected<TextureHandle, StringView> VulkanRenderer::CreateTexture(VulkanTexture& vulkanImage)
    {
        return m_imagesPool.Create(std::move(vulkanImage));
    }

    Expected<BufferHandle, StringView> VulkanRenderer::CreateBuffer(uint32 size, BufferUsage usage,
                                                                    MemoryType memoryType)
    {
        VulkanBuffer buffer;
        buffer.m_usage = VulkanUtils::GetBufferUsageFlags(usage);
        buffer.m_size = size;
        buffer.m_memoryPropertyFlags = VulkanUtils::GetMemoryPropertyFlags(memoryType);
        
        VkBufferCreateInfo bufferCreateInfo{
            .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
            .flags = 0,
            .size = size,
            .usage = static_cast<VkBufferUsageFlags>(buffer.m_usage),
            .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
            .queueFamilyIndexCount = 0,
            .pQueueFamilyIndices = nullptr
        };
        
        VmaAllocationCreateInfo allocCreateInfo{};
        if (buffer.m_memoryPropertyFlags | vk::MemoryPropertyFlagBits::eHostVisible)
        {
            allocCreateInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT;
            allocCreateInfo.requiredFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
            allocCreateInfo.preferredFlags = VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_CACHED_BIT;
        }
        
        allocCreateInfo.usage = VMA_MEMORY_USAGE_AUTO;
        VkBuffer newBuffer = VK_NULL_HANDLE; 
        auto createBufferResults = vmaCreateBufferWithAlignment(Details::g_vmaAllocator, &bufferCreateInfo, &allocCreateInfo, 16, &newBuffer, &buffer.m_allocation, nullptr);
        
        if (createBufferResults != VK_SUCCESS)
        {
            pdlLogError("Failed to allocate buffer with error code %d", createBufferResults);
            return Unexpected<StringView>("Failed to create buffer");
        }
        
        buffer.m_buffer = newBuffer;
        
        if (buffer.m_memoryPropertyFlags | vk::MemoryPropertyFlagBits::eHostVisible) {
            vmaMapMemory(Details::g_vmaAllocator, buffer.m_allocation, &buffer.m_mappedPtr);
        }
        
        if (buffer.m_usage & vk::BufferUsageFlagBits::eShaderDeviceAddress) 
        {
            const vk::BufferDeviceAddressInfo ai(buffer.m_buffer);
            buffer.m_deviceAddress = m_vkDevice.getBufferAddress(&ai);
            pdlAssert(buffer.m_deviceAddress != 0);
        }
        
        return m_buffersPool.Create(std::move(buffer));
    }

    void VulkanRenderer::Destroy(TextureHandle bufferHandle)
    {
        if (!m_imagesPool.IsValid(bufferHandle))
        {
            pdlLogWarning("VulkanRenderer::Destroy: Invalid TextureHandle {0x%p}. Maybe destroyed already?", bufferHandle.GetHandleAsVoid());
            return;
        }
        
        VulkanTexture* texturePtr = m_imagesPool.Get(bufferHandle);
        
        if (texturePtr->m_vkImageView)
            m_vkDevice.destroyImageView(texturePtr->m_vkImageView);

        if (!!(texturePtr->m_flags & VulkanTexture::Flags::OwnsVkImage))
        {
            if (texturePtr->m_vkImage)
            {
                m_vkDevice.destroyImage(texturePtr->m_vkImage);
            }

            if (texturePtr->m_mappedPtr)
            {
                vmaUnmapMemory(Details::g_vmaAllocator, texturePtr->m_vmaAllocation);
            }
        
            if (texturePtr->m_vmaAllocation)
            {
                vmaDestroyImage(Details::g_vmaAllocator, texturePtr->m_vkImage, texturePtr->m_vmaAllocation);
            }
        }
        
        m_imagesPool.Destroy(bufferHandle);
        
    }

    void VulkanRenderer::Destroy(BufferHandle bufferHandle)
    {
        if (!m_buffersPool.IsValid(bufferHandle))
        {
            pdlLogWarning("VulkanRenderer::Destroy: Invalid BufferHandle {0x%p}. Maybe destroyed already?", bufferHandle.GetHandleAsVoid());
            return;
        }
        
        VulkanBuffer* bufferPtr = m_buffersPool.Get(bufferHandle);

        if (bufferPtr->m_mappedPtr) {
            vmaUnmapMemory(Details::g_vmaAllocator, bufferPtr->m_allocation);
        }
        
        m_buffersPool.Destroy(bufferHandle);
        vmaDestroyBuffer(Details::g_vmaAllocator, bufferPtr->m_buffer, bufferPtr->m_allocation);
    }   

    Expected<void, StringView> VulkanRenderer::InitializeInstanceAndDevice(const InitInfo& initInfo)
    {
        // Init minimum set of functions
        VULKAN_HPP_DEFAULT_DISPATCHER.init(vkGetInstanceProcAddr);

        vk::ApplicationInfo applicationInfo(initInfo.m_applicationName.data(), 1, "PardalEngine", 1, VK_API_VERSION_1_4);


        VKEFH::InstanceInitHelp instInitHelp;
        VkResult res = instInitHelp.EnumerateExtensions();
        if (res != VK_SUCCESS)
        {
            return Unexpected<StringView>("Failed to enumerate Vulkan extensions");
        }
        res = instInitHelp.EnumerateLayers();
        if (res != VK_SUCCESS)
        {
            return Unexpected<StringView>("Failed to enumerate Vulkan layers");
        }
        
        if (initInfo.m_enableValidation)
        {
            if (!instInitHelp.IsLayerSupported("VK_LAYER_KHRONOS_validation"))
            {
                return Unexpected(
                   "Layer check failed. Tip: Set the environment variable VK_LAYER_PATH to point to the location of your layers");
            }
        }
        else
        {
            instInitHelp.EnableLayer("VK_LAYER_KHRONOS_validation", false);
        }

        instInitHelp.PrepareCreation();
        
        // Instance creation
        vk::InstanceCreateInfo instanceCreateInfo({}, &applicationInfo, 
            instInitHelp.GetEnabledLayerCount(), 
            instInitHelp.GetEnabledLayerNames(),
            instInitHelp.GetEnabledExtensionCount(), 
            instInitHelp.GetEnabledExtensionNames());
        
        auto instance = vk::createInstance(instanceCreateInfo);
        CHECK_VK_RESULTVALUE(instance);
        m_vkInstance = instance.value;

        // Init all dynamically loaded functions for instance
        VULKAN_HPP_DEFAULT_DISPATCHER.init(instance.value, vkGetInstanceProcAddr);

        // Set up validation messenger
        if (initInfo.m_enableValidation)
        {
            vk::DebugUtilsMessengerCreateInfoEXT debugInfo;
            debugInfo.messageSeverity = vk::DebugUtilsMessageSeverityFlagBitsEXT::eError |
                vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning;
            debugInfo.messageType = vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
                vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation;
            debugInfo.pfnUserCallback = Details::DebugUtilsMessengerCallback;


            auto debugMessenger = instance.value.createDebugUtilsMessengerEXT(debugInfo);
            CHECK_VK_RESULTVALUE(debugMessenger);
            m_vkDebugMessenger = debugMessenger.value;
        }

        // Device creation
        auto enumeratedDevices = m_vkInstance.enumeratePhysicalDevices();
        CHECK_VK_RESULTVALUE(enumeratedDevices);
        auto enumeratedDevicesVector = VectorUtils::FromStd(enumeratedDevices.value);
        m_vkPhysicalDevice = Details::PickBestDevice(enumeratedDevicesVector);

        m_deviceInfo.adapterName = m_vkPhysicalDevice.getProperties().deviceName.data();

        // Collect device properties
        vk::PhysicalDeviceProperties deviceProperties = m_vkPhysicalDevice.getProperties();
        auto& limits = m_deviceInfo.limits;
        limits.maxTextureDimension1D = deviceProperties.limits.maxImageDimension1D;
        limits.maxTextureDimension2D = deviceProperties.limits.maxImageDimension2D;
        limits.maxTextureDimension3D = deviceProperties.limits.maxImageDimension3D;
        limits.maxTextureDimensionCube = deviceProperties.limits.maxImageDimensionCube;
        limits.maxTextureArrayLayers = deviceProperties.limits.maxImageArrayLayers;

        limits.maxVertexInputElements = deviceProperties.limits.maxVertexInputAttributes;
        limits.maxVertexInputElementOffset = deviceProperties.limits.maxVertexInputAttributeOffset;
        limits.maxVertexStreams = deviceProperties.limits.maxVertexInputBindings;
        limits.maxVertexStreamStride = deviceProperties.limits.maxVertexInputBindingStride;

        limits.maxComputeThreadsPerGroup = deviceProperties.limits.maxComputeWorkGroupInvocations;
        limits.maxComputeThreadGroupSize[0] = deviceProperties.limits.maxComputeWorkGroupSize[0];
        limits.maxComputeThreadGroupSize[1] = deviceProperties.limits.maxComputeWorkGroupSize[1];
        limits.maxComputeThreadGroupSize[2] = deviceProperties.limits.maxComputeWorkGroupSize[2];
        limits.maxComputeDispatchThreadGroups[0] = deviceProperties.limits.maxComputeWorkGroupCount[0];
        limits.maxComputeDispatchThreadGroups[1] = deviceProperties.limits.maxComputeWorkGroupCount[1];
        limits.maxComputeDispatchThreadGroups[2] = deviceProperties.limits.maxComputeWorkGroupCount[2];

        limits.maxViewports = deviceProperties.limits.maxViewports;
        limits.maxViewportDimensions[0] = deviceProperties.limits.maxViewportDimensions[0];
        limits.maxViewportDimensions[1] = deviceProperties.limits.maxViewportDimensions[1];
        limits.maxFramebufferDimensions[0] = deviceProperties.limits.maxFramebufferWidth;
        limits.maxFramebufferDimensions[1] = deviceProperties.limits.maxFramebufferHeight;
        limits.maxFramebufferDimensions[2] = deviceProperties.limits.maxFramebufferLayers;

        limits.maxShaderVisibleSamplers = deviceProperties.limits.maxPerStageDescriptorSamplers;
        limits.maxShaderVisibleInputSamplers = deviceProperties.limits.maxPerStageDescriptorInputAttachments;
        
        // Queues
        m_deviceQueues.graphicsQueueFamilyIndex = Details::FindQueue(m_vkPhysicalDevice, vk::QueueFlagBits::eGraphics);
        m_deviceQueues.computeQueueFamilyIndex = Details::FindQueue(m_vkPhysicalDevice, vk::QueueFlagBits::eCompute);
        if (m_deviceQueues.graphicsQueueFamilyIndex < 0)
        {
            return Unexpected("Could not find a suitable graphics family index in the Vulkan physical device.");
        }
        if (m_deviceQueues.computeQueueFamilyIndex < 0)
        {
            return Unexpected("Could not find a suitable compute family index in the Vulkan physical device.");
        }

        float queuePriority = 1.0f;

        vk::DeviceQueueCreateInfo deviceQueueCreateInfo[2] = {
            {vk::DeviceQueueCreateFlags(), static_cast<uint32_t>(m_deviceQueues.graphicsQueueFamilyIndex), 1, &queuePriority},
            {vk::DeviceQueueCreateFlags(), static_cast<uint32_t>(m_deviceQueues.computeQueueFamilyIndex), 1, &queuePriority}
        };
        int numQueues = m_deviceQueues.graphicsQueueFamilyIndex == m_deviceQueues.computeQueueFamilyIndex ? 1 : 2;


        // BEGIN FEATURES AND EXTENSIONS AND ETC
        VKEFH::DeviceInitHelp devInitHelp;
        devInitHelp.GetPhysicalDeviceFeatures(m_vkPhysicalDevice);
        vk::Result enumerateExtensionsResult = static_cast<vk::Result>(devInitHelp.EnumerateExtensions(m_vkPhysicalDevice));
        CHECK_VK_RESULT(enumerateExtensionsResult);
        
        // Configure features
        devInitHelp.GetVkPhysicalDeviceVulkan14Features().pushDescriptor = VK_TRUE;
        
        devInitHelp.PrepareCreation();
        vk::DeviceCreateInfo deviceCreateInfo(vk::DeviceCreateFlags(),
                                            numQueues,
                                            deviceQueueCreateInfo,
                                            0,
                                            nullptr,
                                            devInitHelp.GetEnabledExtensionCount(),
                                            devInitHelp.GetEnabledExtensionNames(),
                                            nullptr,
                                            devInitHelp.GetFeaturesChain()
                                            );

        // Create device
        auto createDeviceResults = m_vkPhysicalDevice.createDevice(deviceCreateInfo);
        CHECK_VK_RESULTVALUE(createDeviceResults);
        m_vkDevice = createDeviceResults.value;

        // Get queues
        m_deviceQueues.graphicsQueue = m_vkDevice.getQueue(m_deviceQueues.graphicsQueueFamilyIndex, 0);
        m_deviceQueues.computeQueue = m_vkDevice.getQueue(m_deviceQueues.computeQueueFamilyIndex, 0);


        // Init all dynamically loaded functions for the created device
        VULKAN_HPP_DEFAULT_DISPATCHER.init(m_vkDevice);

        // Vulkan memory allocator
        auto vmaCreateResults = Details::CreateVmaAllocator(m_vkPhysicalDevice, m_vkDevice, m_vkInstance, applicationInfo.apiVersion);
        if (!vmaCreateResults)
        {
            return Unexpected(vmaCreateResults.error());
        }
        Details::g_vmaAllocator = vmaCreateResults.value();
        
        // Pipeline cache
        Vector<byte> pipelineCacheData;
        if (!initInfo.m_pipelineCachePath.empty())
        {
            pdlNotImplemented();
        }
        vk::PipelineCacheCreateInfo pipelineCacheCreateInfo
        (
            {},
            pipelineCacheData.size(),
            pipelineCacheData.data()
        );

        auto createPipelineCacheResults = m_vkDevice.createPipelineCache(pipelineCacheCreateInfo);
        CHECK_VK_RESULTVALUE(createPipelineCacheResults);
        m_vkPipelineCache = createPipelineCacheResults.value;

        // Surface
#ifdef PDL_PLATFORM_WINDOWS
        vk::Win32SurfaceCreateInfoKHR surfaceCreateInfo = {};
        surfaceCreateInfo.hinstance = static_cast<HINSTANCE>(initInfo.m_applicationWindow.GetNativeModuleHandle());
        surfaceCreateInfo.hwnd = static_cast<HWND>(initInfo.m_applicationWindow.GetNativeWindow());
        auto surfaceCreateResults = m_vkInstance.createWin32SurfaceKHR(surfaceCreateInfo);
        CHECK_VK_RESULTVALUE(surfaceCreateResults);
        m_vkSurface = surfaceCreateResults.value;
#else
#error Implement relevant surface creation for this platform
#endif

        // Essential objects
        
        {
            Array<vk::DescriptorSetLayoutBinding, RenderererConstants::MaxColorAttachments()> layoutBindings{};
            for (uint32_t i = 0; i < layoutBindings.size(); i++)        
            {
                auto& binding = layoutBindings[i];
                binding.binding = i;
                binding.descriptorType = vk::DescriptorType::eInputAttachment;
                binding.descriptorCount = 1;
                binding.stageFlags = vk::ShaderStageFlagBits::eFragment;
            }
            auto maxInputAttachments = Math::Min(static_cast<uint8>(m_deviceInfo.limits.maxShaderVisibleInputSamplers), RenderererConstants::MaxColorAttachments());
            vk::DescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo(vk::DescriptorSetLayoutCreateFlagBits::ePushDescriptor, 
                maxInputAttachments, 
                layoutBindings.data());
            auto descriptorSetLayout = m_vkDevice.createDescriptorSetLayout(descriptorSetLayoutCreateInfo);
            CHECK_VK_RESULTVALUE(descriptorSetLayout);
            m_inputAttachmentDescriptorSetLayout = descriptorSetLayout.value;
        }
        
        m_immediateCommand = MakeUniquePointer<VulkanImmediateCommand>(*this, m_deviceQueues.graphicsQueueFamilyIndex);
        
        auto command = m_immediateCommand->Acquire();
        m_immediateCommand->Submit(command);
        
        return {};
    }
}
