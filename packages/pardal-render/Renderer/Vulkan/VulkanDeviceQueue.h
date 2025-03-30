#pragma once
#include <vulkan/vulkan.hpp>
#include <Base/BaseTypes.h>
#include <Containers/Array.h>

// Created on 2024-11-11 by sisco

namespace pdl
{
    class VulkanTexture;
}

namespace pdl
{

class VulkanDeviceQueue
{
public:
    bool Initialize(vk::Device& device, vk::Queue& queue, int queueIndex);
    void Destroy();

    void Flush();
    void WaitForGPU() const;
    void FlushAndWait();

    void SetImageLayout(const VulkanTexture* texture, vk::ImageLayout oldImageLayout, vk::ImageLayout newImageLayout);
    
    vk::CommandBuffer GetCommandBuffer() const { return *m_currentCommandBuffer; }
    vk::Queue GetQueue() const { return m_queue; }

    void FlushCurrentCommandBuffer();
    void PrepareNextCommandBuffer();

    enum class EventType
    {
        BeginFrame,
        EndFrame,
        //----
        EventCount
    };

    vk::Semaphore GetVkSemaphore(EventType event) const { return m_semaphores[static_cast<int>(event)];} 
    void SetCurrentSemaphore(EventType event); 
private:

    
    struct Fence
    {
        vk::Fence fence;
        bool active = false;
        size_t value = 0;
    };

    bool IsCurrentSemaphore(EventType event) const { return m_currentSemaphores[static_cast<int>(event)] != nullptr;} 
    void CompleteSemaphore(EventType event) { m_currentSemaphores[static_cast<int>(event)] = nullptr; };
    void UpdateFence(size_t fenceIndex, bool wait = false);
    
    static constexpr size_t kCommandBufferCount = 8;
    Array<vk::CommandPool, kCommandBufferCount> m_commandPools;
    Array<vk::CommandBuffer, kCommandBufferCount> m_commandBuffers;
    Array<Fence, kCommandBufferCount> m_fences;
    size_t m_nextFenceValue = 0;
    size_t m_lastFenceCompleted = 0;
    
    Array<vk::Semaphore, static_cast<size_t>(EventType::EventCount)> m_semaphores;
    Array<vk::Semaphore*, static_cast<size_t>(EventType::EventCount)> m_currentSemaphores {};

    vk::Queue m_queue;
    int m_queueIndex = -1;

    size_t m_currentCommandBufferIndex = 0;
    vk::CommandBuffer* m_currentCommandBuffer = nullptr; 
    vk::CommandPool* m_currentCommandPool = nullptr; 

    vk::Device* m_device = nullptr;
    
};

}

