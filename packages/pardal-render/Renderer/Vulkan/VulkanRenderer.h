
#pragma once
#include <vulkan/vulkan.hpp>
#include "Base/BaseDefines.h"
#include "Base/Expected.h"
#include "Containers/Pool.h"
#include "Renderer/IRenderer.h"
#include "Renderer/RenderererInfo.h"
#include "Renderer/Vulkan/VulkanBuffer.h"
#include "Renderer/Vulkan/VulkanTexture.h"


// Created on 2026-01-26 by sisco

namespace pdl
{
    class VulkanImmediateCommand;
    class VulkanSwapchain;

class VulkanRenderer : public IRenderer
{
public:
    struct DeviceQueues 
    {
        static constexpr int32 INVALID = -1;
        int32 graphicsQueueFamilyIndex = INVALID;
        int32 computeQueueFamilyIndex = INVALID;

        vk::Queue graphicsQueue;
        vk::Queue computeQueue;
    };

    VulkanRenderer(const InitInfo& initInfo);
    ~VulkanRenderer() override;

    DeclareNonCopyable(VulkanRenderer);
    
    const RenderDeviceInfo& GetDeviceInfo() const override { return m_deviceInfo; }
    
    Expected<void, StringView> InitSwapchain(uint32 width, uint32 height) override;
    
    Expected<ICommandBuffer*, StringView> GetCommandBuffer() override;
    Expected<void, StringView> SubmitCommandBuffer(ICommandBuffer* commandBuffer, TextureHandle presentTarget) override;

    Expected<TextureHandle, StringView> CreateTexture(VulkanTexture& vulkanImage);
    Expected<BufferHandle, StringView> CreateBuffer(uint32 size, 
                                                    BufferUsage usage,
                                                    MemoryType memoryType) override;

    void Destroy(TextureHandle bufferHandle) override;
    void Destroy(BufferHandle bufferHandle) override;
    
    const vk::PhysicalDevice& GetPhysicalDevice() const { return m_vkPhysicalDevice; }
    const vk::Device& GetDevice() const { return m_vkDevice; }
    const vk::SurfaceKHR& GetSurface() const { return m_vkSurface; }
    const DeviceQueues& GetDeviceQueues() const { return m_deviceQueues; }
        
    // Object getters
    VulkanTexture* Get(const TextureHandle& handle) { return m_imagesPool.Get(handle); }
    VulkanBuffer* Get(const BufferHandle& handle) { return m_buffersPool.Get(handle); }
    
    TextureHandle GetCurrentSwapchainTexture() const override;
private:
    Expected<void, StringView> InitializeInstanceAndDevice(const InitInfo& initInfo);
    
    RenderDeviceInfo m_deviceInfo = {};
    
    vk::Instance m_vkInstance;
    vk::PhysicalDevice m_vkPhysicalDevice;
    vk::Device m_vkDevice;
    vk::DebugUtilsMessengerEXT m_vkDebugMessenger;
    vk::SurfaceKHR m_vkSurface;
    
    vk::PipelineCache m_vkPipelineCache;
    
    
    DeviceQueues m_deviceQueues;
    
    // Pools
    Pool<Buffer, VulkanBuffer> m_buffersPool;
    Pool<Texture, VulkanTexture> m_imagesPool;
    
    // Essential objects
    UniquePointer<VulkanSwapchain> m_swapchain;
    UniquePointer<VulkanImmediateCommand> m_immediateCommand;
    vk::Semaphore m_vkTimelineSemaphore;
    vk::DescriptorSetLayout m_inputAttachmentDescriptorSetLayout;
};

}

