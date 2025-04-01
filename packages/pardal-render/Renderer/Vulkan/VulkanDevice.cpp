#include <Renderer/Vulkan/VulkanDevice.h>

#include <Base/DebugHelpers.h>
#include <Log/Log.h>
#include <Renderer/Vulkan/VulkanUtils.h>
#include <String/StringUtils.h>
#include <Renderer/Vulkan/VulkanSurface.h>
#include <Renderer/Vulkan/VulkanTextureView.h>

#include "VulkanRenderBuffer.h"
#include "backends/imgui_impl_vulkan.h"


#ifdef PDL_PLATFORM_WINDOWS
#include <dxgi1_2.h>
#endif

#ifndef VULKAN_HPP_DISPATCH_LOADER_DYNAMIC
#error VULKAN_HPP_DISPATCH_LOADER_DYNAMIC is required
#endif

VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE

// Created on 2025-03-23 by sisco
namespace Details
{
    static bool CheckLayers(const pdl::Vector<char const *>& layers,const pdl::Vector<vk::LayerProperties>& properties )
    {
        // return true if all layers are listed in the properties
        return std::ranges::all_of(layers, [&properties]( char const * name )
        {
           bool found = std::ranges::any_of(properties, [&name]( vk::LayerProperties const & property ) { return strcmp( property.layerName, name ) == 0; } );
            if(!found)
            {
                pdlLogError("Vulkan Layer not found: %s", name);    
            }
            return found;
        });
    }

    static vk::PhysicalDevice& PickBestDevice(pdl::Vector<vk::PhysicalDevice>& devices)
    {
        pdlAssert(!devices.empty());
        uint32 bestDeviceIndex = 0;
        size_t mostVRAMfound = 0;

        // Find the best one if there's more than one
        if(devices.size() > 1)
        {
            for (uint32 deviceIndex = 0; deviceIndex < devices.size(); ++deviceIndex)
            {
                auto& device = devices.at(deviceIndex);
                auto deviceProperties = device.getProperties();

                // Always favor discrete gpus over anything else
                if (devices[bestDeviceIndex].getProperties().deviceType != vk::PhysicalDeviceType::eDiscreteGpu &&
                    deviceProperties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu)
                {
                    bestDeviceIndex = deviceIndex;
                    continue;
                }

                // Next best indicator is memory
                auto memoryProperties = device.getMemoryProperties();
                for (auto& heap : memoryProperties.memoryHeaps)
                {
                    if(heap.flags & vk::MemoryHeapFlagBits::eDeviceLocal)
                    {
                        if(heap.size > mostVRAMfound)
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
    
    static VkBool32 DebugUtilsMessengerCallback( vk::DebugUtilsMessageSeverityFlagBitsEXT       messageSeverity,
                                                            vk::DebugUtilsMessageTypeFlagsEXT              messageTypes,
                                                            vk::DebugUtilsMessengerCallbackDataEXT const * pCallbackData,
                                                            void * /*pUserData*/ )
    {
        pdl::String debugMessage = pdl::StringUtils::StringFormat("[Vulkan %s] %s: %s [ID%s%d]",
                vk::to_string(messageSeverity).c_str(),
                vk::to_string(messageTypes).c_str(),
                pCallbackData->pMessage,
                pCallbackData->pMessageIdName,
                pCallbackData->messageIdNumber       
        );

        if(pCallbackData->queueLabelCount > 0)
        {
            debugMessage += "\n{Queues: ";
            for ( uint32_t i = 0; i < pCallbackData->queueLabelCount; i++ )
            {
                debugMessage += pCallbackData->pQueueLabels[i].pLabelName;
                debugMessage += " ";
            }
            debugMessage += "}";
        }

        if(pCallbackData->cmdBufLabelCount  > 0)
        {
            debugMessage += "\n{CmdBufs: ";
            for ( uint32_t i = 0; i < pCallbackData->cmdBufLabelCount ; i++ )
            {
                debugMessage += pCallbackData->pCmdBufLabels[i].pLabelName;
                debugMessage += " ";
            }
            debugMessage += "}";
        }

        if(pCallbackData->objectCount  > 0)
        {
            debugMessage += "\n{Objects: ";
            for ( uint32_t i = 0; i < pCallbackData->objectCount ; i++ )
            {
                debugMessage += pdl::StringUtils::StringFormat("%s %llx [%s],  ",
                    vk::to_string(static_cast<vk::ObjectType>( pCallbackData->pObjects[i].objectType)).c_str(),
                    pCallbackData->pObjects[i].objectHandle,
                    pCallbackData->pObjects[i].pObjectName ? pCallbackData->pObjects[i].pObjectName : "???");
            }
            debugMessage += "}";
        }


        
        if(messageSeverity & vk::DebugUtilsMessageSeverityFlagBitsEXT::eError)
        {
            pdlLogError(debugMessage.c_str());
        }
        else if(messageSeverity & vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning)
        {
            pdlLogWarning(debugMessage.c_str());
        }
        else
        {
            pdlLogInfo(debugMessage.c_str());
        }

        return false;
    }

    static int FindQueue(const vk::PhysicalDevice& device, vk::QueueFlags queueFlags)
    {
        for(uint32 i = 0; i < device.getQueueFamilyProperties().size(); i++)
        {
            if(device.getQueueFamilyProperties()[i].queueFlags & queueFlags)
            {
                return i;
            }
        }
        return -1;
    }

    inline bool IsExtensionInExtensionProperties(pdl::StringView extension, const pdl::Vector<vk::ExtensionProperties>& extensionProperties)
    {
        return (extensionProperties.end() != std::ranges::find_if(extensionProperties, [&extension](auto& val) { return val.extensionName == extension;}));
    }
    inline void EnumerateAllExtensionsAndFeatures(vk::PhysicalDevice& device, pdl::Vector<const char*>& extensions, pdl::Vector<const char*>& features, const pdl::Vector<vk::ExtensionProperties>& extensionProperties)
    {

        extensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
        extensions.push_back(VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME);
        
        auto deviceFeatures2 = device.getFeatures2<
            vk::PhysicalDeviceFeatures2,
            vk::PhysicalDevice16BitStorageFeatures,
            vk::PhysicalDevice8BitStorageFeaturesKHR,
            vk::PhysicalDeviceASTCDecodeFeaturesEXT,
            vk::PhysicalDeviceBlendOperationAdvancedFeaturesEXT,
            vk::PhysicalDeviceBufferDeviceAddressFeaturesEXT,
            vk::PhysicalDeviceCoherentMemoryFeaturesAMD,
            vk::PhysicalDeviceComputeShaderDerivativesFeaturesNV,
            vk::PhysicalDeviceConditionalRenderingFeaturesEXT,
            vk::PhysicalDeviceCooperativeMatrixFeaturesNV,
            vk::PhysicalDeviceCornerSampledImageFeaturesNV,
            vk::PhysicalDeviceCoverageReductionModeFeaturesNV,
            vk::PhysicalDeviceDedicatedAllocationImageAliasingFeaturesNV,
            vk::PhysicalDeviceDepthClipEnableFeaturesEXT,
            vk::PhysicalDeviceDescriptorIndexingFeaturesEXT,
            vk::PhysicalDeviceExclusiveScissorFeaturesNV,
            vk::PhysicalDeviceFragmentDensityMapFeaturesEXT,
            vk::PhysicalDeviceFragmentShaderBarycentricFeaturesNV,
            vk::PhysicalDeviceFragmentShaderInterlockFeaturesEXT,
            vk::PhysicalDeviceHostQueryResetFeaturesEXT,
            vk::PhysicalDeviceImagelessFramebufferFeaturesKHR,
            vk::PhysicalDeviceIndexTypeUint8FeaturesEXT,
            vk::PhysicalDeviceInlineUniformBlockFeaturesEXT,
            vk::PhysicalDeviceLineRasterizationFeaturesEXT,
            vk::PhysicalDeviceMemoryPriorityFeaturesEXT,
            vk::PhysicalDeviceMeshShaderFeaturesNV,
            vk::PhysicalDeviceMultiviewFeatures,
            vk::PhysicalDevicePipelineExecutablePropertiesFeaturesKHR,
            vk::PhysicalDeviceProtectedMemoryFeatures,
            vk::PhysicalDeviceRepresentativeFragmentTestFeaturesNV,
            vk::PhysicalDeviceSamplerYcbcrConversionFeatures,
            vk::PhysicalDeviceScalarBlockLayoutFeaturesEXT,
            vk::PhysicalDeviceShaderAtomicInt64FeaturesKHR,
            vk::PhysicalDeviceShaderDemoteToHelperInvocationFeaturesEXT,
            vk::PhysicalDeviceShaderDrawParametersFeatures,
            vk::PhysicalDeviceShaderFloat16Int8FeaturesKHR,
            vk::PhysicalDeviceShaderImageFootprintFeaturesNV,
            vk::PhysicalDeviceShaderIntegerFunctions2FeaturesINTEL,
            vk::PhysicalDeviceShaderSMBuiltinsFeaturesNV,
            vk::PhysicalDeviceShaderSubgroupExtendedTypesFeaturesKHR,
            vk::PhysicalDeviceShadingRateImageFeaturesNV,
            vk::PhysicalDeviceSubgroupSizeControlFeaturesEXT,
            vk::PhysicalDeviceTexelBufferAlignmentFeaturesEXT,
            vk::PhysicalDeviceTextureCompressionASTCHDRFeaturesEXT,
            vk::PhysicalDeviceTimelineSemaphoreFeaturesKHR,
            vk::PhysicalDeviceTransformFeedbackFeaturesEXT,
            vk::PhysicalDeviceUniformBufferStandardLayoutFeaturesKHR,
            vk::PhysicalDeviceVariablePointersFeatures,
            vk::PhysicalDeviceVertexAttributeDivisorFeaturesEXT,
            vk::PhysicalDeviceVulkanMemoryModelFeaturesKHR,
            vk::PhysicalDeviceYcbcrImageArraysFeaturesEXT,
            vk::PhysicalDeviceAccelerationStructureFeaturesKHR,
            vk::PhysicalDeviceRayQueryFeaturesKHR,
            vk::PhysicalDeviceRayTracingPipelineFeaturesKHR,
            vk::PhysicalDeviceRobustness2FeaturesEXT,
            vk::PhysicalDeviceShaderClockFeaturesKHR,
            vk::PhysicalDeviceVulkan12Features,
            vk::PhysicalDeviceFragmentShadingRateFeaturesKHR,
            vk::PhysicalDeviceRayTracingInvocationReorderFeaturesNV,
            vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT,
            vk::PhysicalDeviceShaderImageAtomicInt64FeaturesEXT,
            vk::PhysicalDeviceShaderAtomicFloat2FeaturesEXT,
            vk::PhysicalDeviceShaderAtomicFloatFeaturesEXT
        >();

        if (deviceFeatures2.get<vk::PhysicalDeviceFeatures2>().features.shaderResourceMinLod)
        {
            features.push_back("shader-resource-min-lod");
        }
        if (deviceFeatures2.get<vk::PhysicalDeviceFeatures2>().features.shaderFloat64)
        {
            features.push_back("double");
        }
        if (deviceFeatures2.get<vk::PhysicalDeviceFeatures2>().features.shaderInt64)
        {
            features.push_back("int64");
        }
        if (deviceFeatures2.get<vk::PhysicalDeviceFeatures2>().features.shaderInt16)
        {
            features.push_back("int16");
        }
        // If we have float16 features then enable
        if (deviceFeatures2.get<vk::PhysicalDeviceShaderFloat16Int8Features>().shaderFloat16)
        {
            // We have half support
            features.push_back("half");
        }

        const auto addFeatureExtension =
            [&](const bool feature, const char* extension = nullptr)
        {
            if (!feature)
                return false;
            if (extension)
            {
                if(!IsExtensionInExtensionProperties(extension, extensionProperties))
                    return false;
                extensions.push_back(extension);
            }
            return true;
        };

        // SIMPLE_EXTENSION_FEATURE(struct, feature member name, extension
        // name, features...) will check for the presence of the boolean
        // feature member in struct and the availability of the extensions. If
        // they are both present then the extensions are added, the struct
        // linked into the deviceCreateInfo chain and the features added to the
        // supported features list.
#define SIMPLE_EXTENSION_FEATURE(s, m, e, ...) \
    do                                         \
    {                                          \
        const static auto fs = {__VA_ARGS__};  \
        if (addFeatureExtension(deviceFeatures2.get<s>().m, e))    \
            for (const auto& p : fs)           \
                features.push_back(p);             \
    } while (0)

        SIMPLE_EXTENSION_FEATURE(
            vk::PhysicalDevice16BitStorageFeatures,
            storageBuffer16BitAccess,
            VK_KHR_16BIT_STORAGE_EXTENSION_NAME,
            "16-bit-storage");

        SIMPLE_EXTENSION_FEATURE(
            vk::PhysicalDeviceShaderAtomicFloatFeaturesEXT,
            shaderBufferFloat32Atomics,
            VK_EXT_SHADER_ATOMIC_FLOAT_EXTENSION_NAME,
            "atomic-float");

        SIMPLE_EXTENSION_FEATURE(
            vk::PhysicalDeviceShaderAtomicFloat2FeaturesEXT,
            shaderBufferFloat16Atomics,
            VK_EXT_SHADER_ATOMIC_FLOAT_2_EXTENSION_NAME,
            "atomic-float-2");

        SIMPLE_EXTENSION_FEATURE(
            vk::PhysicalDeviceShaderImageAtomicInt64FeaturesEXT,
            shaderImageInt64Atomics,
            VK_EXT_SHADER_IMAGE_ATOMIC_INT64_EXTENSION_NAME,
            "image-atomic-int64");

        SIMPLE_EXTENSION_FEATURE(
            vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT,
            extendedDynamicState,
            VK_EXT_EXTENDED_DYNAMIC_STATE_EXTENSION_NAME,
            "extended-dynamic-states");

        if (deviceFeatures2.get<vk::PhysicalDeviceAccelerationStructureFeaturesKHR>().accelerationStructure &&
            IsExtensionInExtensionProperties(VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME, extensionProperties) &&
            IsExtensionInExtensionProperties(VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME, extensionProperties))
        {
            extensions.push_back(VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME);
            extensions.push_back(VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME);
            features.push_back("acceleration-structure");

            // These both depend on VK_KHR_acceleration_structure

            SIMPLE_EXTENSION_FEATURE(
                vk::PhysicalDeviceRayQueryFeaturesKHR,
                rayQuery,
                VK_KHR_RAY_QUERY_EXTENSION_NAME,
                "ray-query",
                "ray-tracing");

            SIMPLE_EXTENSION_FEATURE(
                vk::PhysicalDeviceRayTracingPipelineFeaturesKHR,
                rayTracingPipeline,
                VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME,
                "ray-tracing-pipeline");
        }

        SIMPLE_EXTENSION_FEATURE(
            vk::PhysicalDeviceInlineUniformBlockFeaturesEXT ,
            inlineUniformBlock,
            VK_EXT_INLINE_UNIFORM_BLOCK_EXTENSION_NAME,
            "inline-uniform-block", );

        SIMPLE_EXTENSION_FEATURE(
            vk::PhysicalDeviceRobustness2FeaturesEXT,
            nullDescriptor,
            VK_EXT_ROBUSTNESS_2_EXTENSION_NAME,
            "robustness2", );

        SIMPLE_EXTENSION_FEATURE(
            vk::PhysicalDeviceShaderClockFeaturesKHR,
            shaderDeviceClock,
            VK_KHR_SHADER_CLOCK_EXTENSION_NAME,
            "realtime-clock");

        SIMPLE_EXTENSION_FEATURE(
            vk::PhysicalDeviceMeshShaderFeaturesNV,
            meshShader,
            VK_EXT_MESH_SHADER_EXTENSION_NAME,
            "mesh-shader");

        SIMPLE_EXTENSION_FEATURE(
            vk::PhysicalDeviceMultiviewFeatures,
            multiview,
            VK_KHR_MULTIVIEW_EXTENSION_NAME,
            "multiview");

        SIMPLE_EXTENSION_FEATURE(
            vk::PhysicalDeviceFragmentShadingRateFeaturesKHR,
            primitiveFragmentShadingRate,
            VK_KHR_FRAGMENT_SHADING_RATE_EXTENSION_NAME,
            "fragment-shading-rate");

        SIMPLE_EXTENSION_FEATURE(
            vk::PhysicalDeviceRayTracingInvocationReorderFeaturesNV,
            rayTracingInvocationReorder,
            VK_NV_RAY_TRACING_INVOCATION_REORDER_EXTENSION_NAME,
            "shader-execution-reorder");

        SIMPLE_EXTENSION_FEATURE(
            vk::PhysicalDeviceVariablePointersFeatures,
            variablePointers,
            VK_KHR_VARIABLE_POINTERS_EXTENSION_NAME,
            "variable-pointer");

        SIMPLE_EXTENSION_FEATURE(
            vk::PhysicalDeviceComputeShaderDerivativesFeaturesNV,
            computeDerivativeGroupLinear,
            VK_NV_COMPUTE_SHADER_DERIVATIVES_EXTENSION_NAME,
            "computeDerivativeGroupLinear");

#undef SIMPLE_EXTENSION_FEATURE

        if (deviceFeatures2.get<vk::PhysicalDeviceShaderAtomicInt64Features>().shaderBufferInt64Atomics)
            features.push_back("atomic-int64");

        if (deviceFeatures2.get<vk::PhysicalDeviceTimelineSemaphoreFeatures>().timelineSemaphore)
            features.push_back("timeline-semaphore");

        if (deviceFeatures2.get<vk::PhysicalDeviceVulkan12Features>().shaderSubgroupExtendedTypes)
            features.push_back("shader-subgroup-extended-types");

        if (deviceFeatures2.get<vk::PhysicalDeviceVulkan12Features>().bufferDeviceAddress)
            features.push_back("buffer-device-address");

        // // Approx. DX12 waveops features
        VkPhysicalDeviceProperties2 extendedProps = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2};
        VkPhysicalDeviceRayTracingPipelinePropertiesKHR rtProps = {
            VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_PROPERTIES_KHR
        };
        VkPhysicalDeviceSubgroupProperties subgroupProps = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SUBGROUP_PROPERTIES};

        rtProps.pNext = extendedProps.pNext;
        extendedProps.pNext = &rtProps;
        subgroupProps.pNext = extendedProps.pNext;
        extendedProps.pNext = &subgroupProps;

        vkGetPhysicalDeviceProperties2(device, &extendedProps);
        // if (deviceFeatures2.get<vk::PhysicalDeviceSubgroupProperties>().supportedOperations &
        //     vk::SubgroupFeatureFlagBits::eBasic | vk::SubgroupFeatureFlagBits::eVote | vk::SubgroupFeatureFlagBits::eArithmetic |
        //     vk::SubgroupFeatureFlagBits::eBallot | vk::SubgroupFeatureFlagBits::eShuffle | vk::SubgroupFeatureFlagBits::eShuffleRelative |
        //     vk::SubgroupFeatureFlagBits::eClustered | vk::SubgroupFeatureFlagBits::eQuad |vk::SubgroupFeatureFlagBits::ePartitionedNV)
        //     features.push_back("wave-ops");
        // Approximate DX12's WaveOps boolean
        if (subgroupProps.supportedOperations &
            (VK_SUBGROUP_FEATURE_BASIC_BIT | VK_SUBGROUP_FEATURE_VOTE_BIT | VK_SUBGROUP_FEATURE_ARITHMETIC_BIT |
             VK_SUBGROUP_FEATURE_BALLOT_BIT | VK_SUBGROUP_FEATURE_SHUFFLE_BIT |
             VK_SUBGROUP_FEATURE_SHUFFLE_RELATIVE_BIT | VK_SUBGROUP_FEATURE_CLUSTERED_BIT |
             VK_SUBGROUP_FEATURE_QUAD_BIT | VK_SUBGROUP_FEATURE_PARTITIONED_BIT_NV))
        {
            features.push_back("wave-ops");
        }

        if (IsExtensionInExtensionProperties("VK_KHR_external_memory", extensionProperties))
        {
            extensions.push_back(VK_KHR_EXTERNAL_MEMORY_EXTENSION_NAME);
#if SLANG_WINDOWS_FAMILY
            if (IsExtensionInExtensionProperties("VK_KHR_external_memory_win32", extensionProperties))
            {
                extensions.push_back(VK_KHR_EXTERNAL_MEMORY_WIN32_EXTENSION_NAME);
            }
#else
            if (IsExtensionInExtensionProperties("VK_KHR_external_memory_fd", extensionProperties))
            {
                extensions.push_back(VK_KHR_EXTERNAL_MEMORY_FD_EXTENSION_NAME);
            }
#endif
            features.push_back("external-memory");
        }
        if (IsExtensionInExtensionProperties(VK_KHR_EXTERNAL_SEMAPHORE_EXTENSION_NAME, extensionProperties))
        {
            extensions.push_back(VK_KHR_EXTERNAL_SEMAPHORE_EXTENSION_NAME);
#if SLANG_WINDOWS_FAMILY
            if (IsExtensionInExtensionProperties(VK_KHR_EXTERNAL_SEMAPHORE_WIN32_EXTENSION_NAME, extensionProperties))
            {
                extensions.push_back(VK_KHR_EXTERNAL_SEMAPHORE_WIN32_EXTENSION_NAME);
            }
#else
            if (IsExtensionInExtensionProperties(VK_KHR_EXTERNAL_SEMAPHORE_FD_EXTENSION_NAME, extensionProperties))
            {
                extensions.push_back(VK_KHR_EXTERNAL_SEMAPHORE_FD_EXTENSION_NAME);
            }
#endif
            features.push_back("external-semaphore");
        }
        if (IsExtensionInExtensionProperties(VK_EXT_CONSERVATIVE_RASTERIZATION_EXTENSION_NAME, extensionProperties))
        {
            extensions.push_back(VK_EXT_CONSERVATIVE_RASTERIZATION_EXTENSION_NAME);
            features.push_back("conservative-rasterization-3");
            features.push_back("conservative-rasterization-2");
            features.push_back("conservative-rasterization-1");
        }
        if (IsExtensionInExtensionProperties(VK_EXT_DEBUG_REPORT_EXTENSION_NAME, extensionProperties))
        {
            extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
            if (IsExtensionInExtensionProperties(VK_EXT_DEBUG_MARKER_EXTENSION_NAME, extensionProperties))
            {
                extensions.push_back(VK_EXT_DEBUG_MARKER_EXTENSION_NAME);
            }
        }
        if (IsExtensionInExtensionProperties(VK_EXT_SHADER_VIEWPORT_INDEX_LAYER_EXTENSION_NAME, extensionProperties))
        {
            extensions.push_back(VK_EXT_SHADER_VIEWPORT_INDEX_LAYER_EXTENSION_NAME);
        }
        if (IsExtensionInExtensionProperties(VK_NVX_BINARY_IMPORT_EXTENSION_NAME, extensionProperties))
        {
            extensions.push_back(VK_NVX_BINARY_IMPORT_EXTENSION_NAME);
            features.push_back("nvx-binary-import");
        }
        if (IsExtensionInExtensionProperties(VK_NVX_IMAGE_VIEW_HANDLE_EXTENSION_NAME, extensionProperties))
        {
            extensions.push_back(VK_NVX_IMAGE_VIEW_HANDLE_EXTENSION_NAME);
            features.push_back("nvx-image-view-handle");
        }
        if (IsExtensionInExtensionProperties(VK_KHR_PUSH_DESCRIPTOR_EXTENSION_NAME, extensionProperties))
        {
            extensions.push_back(VK_KHR_PUSH_DESCRIPTOR_EXTENSION_NAME);
            features.push_back("push-descriptor");
        }
        if (IsExtensionInExtensionProperties(VK_NV_FRAGMENT_SHADER_BARYCENTRIC_EXTENSION_NAME, extensionProperties))
        {
            extensions.push_back(VK_NV_FRAGMENT_SHADER_BARYCENTRIC_EXTENSION_NAME);
            features.push_back("barycentrics");
        }
        if (IsExtensionInExtensionProperties(VK_NV_SHADER_SUBGROUP_PARTITIONED_EXTENSION_NAME, extensionProperties))
        {
            extensions.push_back(VK_NV_SHADER_SUBGROUP_PARTITIONED_EXTENSION_NAME);
            features.push_back("shader-subgroup-partitioned");
        }

        // Derive approximate DX12 shader model.
        const char* featureTable[] = {
            "sm_6_0",
            "wave-ops",
            "atomic-int64",
            nullptr,
            "sm_6_1",
            "barycentrics",
            "multiview",
            nullptr,
            "sm_6_2",
            "half",
            nullptr,
            "sm_6_3",
            "ray-tracing-pipeline",
            nullptr,
            "sm_6_4",
            "fragment-shading-rate",
            nullptr,
            "sm_6_5",
            "ray-query",
            "mesh-shader",
            nullptr,
            "sm_6_6",
            "wave-ops",
            "atomic-float",
            "atomic-int64",
            nullptr,
            nullptr,
        };

        int i = 0;
        while (i < sizeof(featureTable))
        {
            const char* sm = featureTable[i++];
            if (sm == nullptr)
            {
                break;
            }
            bool hasAll = true;
            while (i < sizeof(featureTable))
            {
                const char* feature = featureTable[i++];
                if (feature == nullptr)
                {
                    break;
                }
                hasAll &= std::ranges::find(features, feature) != features.end();
            }
            if (hasAll)
            {
                features.push_back(sm);
            }
            else
            {
                break;
            }
        }
        
        features.push_back("hardware-device");
    }
}

namespace pdl
{
    void VulkanDevice::WaitForGPU()
    {
        m_vulkanDeviceQueue.Flush();
        m_vulkanDeviceQueue.WaitForGPU();
    }

    Expected<SharedPointer<ISurface>, StringView> VulkanDevice::CreateSurface(ApplicationWindow& applicationWindow)
    {
        auto surfacePtr = MakeSharedPointer<VulkanSurface>();
        bool success = surfacePtr->Initialize(&m_vkDevice, &m_vkPhysicalDevice, &m_vkInstance, m_vulkanDeviceQueue,applicationWindow, Format::R8G8B8A8_UNORM);
        if (!success)
        {
            return Unexpected<StringView>("Failed to initialize Vulkan surface");
        }
        return  surfacePtr;
    }

    Expected<SharedPointer<ITexture>, StringView> VulkanDevice::CreateTexture(ITexture::TextureDescriptor _textureDescriptor)
    {
        auto vulkanTexture = MakeSharedPointer<VulkanTexture>(_textureDescriptor, &m_vkDevice);
        if (!vulkanTexture->Initialize(&m_vkPhysicalDevice))
        {
            return Unexpected<StringView>("Failed to initialize Vulkan texture");
        }
        return vulkanTexture;
    }

    Expected<SharedPointer<ITextureView>, StringView> VulkanDevice::CreateTextureView(
        ITextureView::TextureViewDescriptor _textureDescriptor)
    {
        auto vulkanTextureView = MakeSharedPointer<VulkanTextureView>(_textureDescriptor, &m_vkDevice);
        return vulkanTextureView;
    }

    Expected<SharedPointer<IRenderBuffer>, StringView> VulkanDevice::CreateRenderBuffer(IRenderBuffer::BufferDescriptor _bufferDescriptor)
    {
        auto vulkanBuffer = MakeSharedPointer<VulkanRenderBuffer>(_bufferDescriptor, m_vkDevice, m_vkPhysicalDevice);
        return vulkanBuffer;
        
    }

    void VulkanDevice::FillImGuiInitInfo(ImGui_ImplVulkan_InitInfo& initInfo)
    {
        initInfo.Device = m_vkDevice;
        initInfo.ApiVersion = VK_API_VERSION_1_3;
        initInfo.Instance = m_vkInstance;
        initInfo.PhysicalDevice = m_vkPhysicalDevice;
        initInfo.QueueFamily = Details::FindQueue( m_vkPhysicalDevice, vk::QueueFlagBits::eGraphics | vk::QueueFlagBits::eCompute);;
        initInfo.Queue = m_vulkanDeviceQueue.GetQueue();
        initInfo.DescriptorPoolSize = 8;
        initInfo.MinImageCount = 2;              // >= 2
        initInfo.ImageCount = 8;                 // >= MinImageCount
        initInfo.MSAASamples = VK_SAMPLE_COUNT_1_BIT;                // 0 defaults to VK_SAMPLE_COUNT_1_BIT
        initInfo.UseDynamicRendering = true;
        initInfo.PipelineRenderingCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO_KHR;
    }

    bool VulkanDevice::Initialize(const InitInfoBase& initInfo)
    {
        m_deviceInfo.name = "pdl::VulkanDevice";
        m_deviceInfo.identityProjection = glm::identity<Math::Matrix44>();
        m_deviceInfo.deviceType = initInfo.m_deviceType;

        if(!InitializeInstanceAndDevice(initInfo))
        {
            pdlLogError("Vulkan device initialization failed.");
            return false;
        }

#ifdef PDL_DEBUG
        pdlLogInfo("Render Device name: %s", m_deviceInfo.name.c_str());
        pdlLogInfo("Adapter: %s", m_deviceInfo.adapterName.c_str());
        pdlLogFlush();
#endif
        return true;
    }

    bool VulkanDevice::InitializeInstanceAndDevice(const InitInfoBase& initInfo)
    {
        // Init minimum set of functions
        VULKAN_HPP_DEFAULT_DISPATCHER.init( vkGetInstanceProcAddr );
        
        vk::ApplicationInfo applicationInfo( initInfo.m_applicationName.data(), 1, "LyraEngine", 1, VK_API_VERSION_1_4 );

        // Validation layers
        auto instanceLayerProperties = vk::enumerateInstanceLayerProperties();
        CHECK_VK_RESULTVALUE(instanceLayerProperties);
        Vector<const char*> instanceLayerNames;
        if(initInfo.m_enableValidation)
            instanceLayerNames.push_back( "VK_LAYER_KHRONOS_validation" );
        
        if ( !Details::CheckLayers( instanceLayerNames, instanceLayerProperties.value ) )
        {
            pdlLogError("Layer check failed. Tip: Set the environment variable VK_LAYER_PATH to point to the location of your layers");
            return false;
        }

        // Extensions
        Vector<const char*> instanceExtensionNames;
        if(initInfo.m_enableValidation)
        {
            instanceExtensionNames.push_back( VK_EXT_DEBUG_REPORT_EXTENSION_NAME );
            instanceExtensionNames.push_back( VK_EXT_DEBUG_UTILS_EXTENSION_NAME );
        }

        instanceExtensionNames.push_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
        instanceExtensionNames.push_back(VK_KHR_EXTERNAL_MEMORY_CAPABILITIES_EXTENSION_NAME);
        instanceExtensionNames.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
        
#ifdef VK_USE_PLATFORM_WIN32_KHR
        instanceExtensionNames.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#else
    #error Implement the correct surface extension for this platform
#endif
        
        // Instance creation
        vk::InstanceCreateInfo instanceCreateInfo( {}, &applicationInfo, instanceLayerNames, instanceExtensionNames );
        auto instance = vk::createInstance( instanceCreateInfo );
        CHECK_VK_RESULTVALUE(instance);
        m_vkInstance = instance.value;

        // Init all dynamically loaded functions for instance
        VULKAN_HPP_DEFAULT_DISPATCHER.init( instance.value, vkGetInstanceProcAddr );

        // Set up validation messenger
        if(initInfo.m_enableValidation)
        {
            vk::DebugUtilsMessengerCreateInfoEXT debugInfo;
            debugInfo.messageSeverity = vk::DebugUtilsMessageSeverityFlagBitsEXT::eError | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning;
            debugInfo.messageType = vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation;
            debugInfo.pfnUserCallback = Details::DebugUtilsMessengerCallback;

        
            auto debugMessenger = instance.value.createDebugUtilsMessengerEXT(debugInfo);
            CHECK_VK_RESULTVALUE(debugMessenger);
            m_vkDebugMessenger = debugMessenger.value;
        }

        // Device creation
        auto enumeratedDevices = m_vkInstance.enumeratePhysicalDevices();
        CHECK_VK_RESULTVALUE(enumeratedDevices);
        m_vkPhysicalDevice = Details::PickBestDevice(enumeratedDevices.value);

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

        // Configure all available extensions
        Vector<const char*> deviceExtensionNames;
        Vector<const char*> deviceFeatures;
        auto extensionProperties = m_vkPhysicalDevice.enumerateDeviceExtensionProperties();
        Details::EnumerateAllExtensionsAndFeatures( m_vkPhysicalDevice, deviceExtensionNames, deviceFeatures, extensionProperties.value );

        // Queues
        int queueFamilyIndex = Details::FindQueue( m_vkPhysicalDevice, vk::QueueFlagBits::eGraphics | vk::QueueFlagBits::eCompute);
        if (queueFamilyIndex < 0)
        {
            pdlLogError("Could not find a suitable family index in the Vulkan physical device.");
            return false;
        }

        float queuePriority = 0.0f;

        // Chekc for dynamic rendering capabilities

        
        vk::DeviceQueueCreateInfo deviceQueueCreateInfo( vk::DeviceQueueCreateFlags(), static_cast<uint32_t>( queueFamilyIndex ), 1, &queuePriority );
        vk::DeviceCreateInfo deviceCreateInfo(vk::DeviceCreateFlags(), deviceQueueCreateInfo, {}, deviceExtensionNames);

        
        constexpr VkPhysicalDeviceDynamicRenderingFeaturesKHR dynamicRenderingFeature
        {
            .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES_KHR,
            .dynamicRendering = VK_TRUE,
        };
        if(std::ranges::find(deviceExtensionNames, VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME) != deviceExtensionNames.end())
        {
            deviceCreateInfo.pNext = &dynamicRenderingFeature;
        }

        auto createDeviceResults = m_vkPhysicalDevice.createDevice( deviceCreateInfo );
        CHECK_VK_RESULTVALUE(createDeviceResults);
        m_vkDevice = createDeviceResults.value;
        // Init all dynamically loaded functions for device
        VULKAN_HPP_DEFAULT_DISPATCHER.init(m_vkDevice);

        auto vkQueue = m_vkDevice.getQueue(queueFamilyIndex, 0);
        if (!m_vulkanDeviceQueue.Initialize(m_vkDevice, vkQueue, queueFamilyIndex))
            return false;

        
        return true;
    }

}

