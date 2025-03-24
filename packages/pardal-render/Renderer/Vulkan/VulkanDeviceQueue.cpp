#include <Renderer/Vulkan/VulkanDeviceQueue.h>
#include <Renderer/Vulkan/VulkanUtils.h>
#include <Math/Functions.h>
#include <algorithm>


// Created on 2024-11-11 by sisco

namespace pdl
{
    bool VulkanDeviceQueue::Initialize(vk::Device& device, vk::Queue& queue, int queueIndex) 
    {
        m_device = &device;
        m_queue = queue;
        m_queueIndex = queueIndex;
        
        for (size_t i = 0; i < kCommandBufferCount; i++)
        {
            vk::CommandPoolCreateInfo commandPoolCreateInfo( vk::CommandPoolCreateFlagBits::eResetCommandBuffer, queueIndex);
            auto createPoolResult = device.createCommandPool(commandPoolCreateInfo);
            CHECK_VK_RESULTVALUE(createPoolResult);
            m_commandPools[i] = createPoolResult.value;

            vk::CommandBufferAllocateInfo allocateInfo = {m_commandPools[i], vk::CommandBufferLevel::ePrimary, 1};
            auto createCommandBufferResult = device.allocateCommandBuffers(allocateInfo);
            CHECK_VK_RESULTVALUE(createCommandBufferResult);
            pdlAssert(createCommandBufferResult.value.size() == 1);
            m_commandBuffers[i] = createCommandBufferResult.value.front();
        
            vk::FenceCreateInfo fenceCreateInfo = {};
            auto createFenceResult = device.createFence(fenceCreateInfo);
            CHECK_VK_RESULTVALUE(createFenceResult);
            m_fences[i].fence = createFenceResult.value;
        }

        for(size_t i = 0; i < static_cast<size_t>(EventType::EventCount); i++)
        {
            vk::SemaphoreCreateInfo semaphoreCreateInfo = {};
            auto createSemaphoreResult = device.createSemaphore(semaphoreCreateInfo);
            CHECK_VK_RESULTVALUE(createSemaphoreResult);
            m_semaphores[i] = createSemaphoreResult.value;
        }

        PrepareNextCommandBuffer();
        
        return true;
    }

    void VulkanDeviceQueue::Destroy()
    {
        for (int i = 0; i < static_cast<int>(EventType::EventCount); ++i)
        {
            m_device->destroySemaphore(m_semaphores[i], nullptr);
        }
        for (size_t i = 0; i < kCommandBufferCount; i++)
        {
            m_device->destroyFence(m_fences[i].fence);
            m_device->freeCommandBuffers(m_commandPools[i], m_commandBuffers[i]);
            m_device->destroyCommandPool(m_commandPools[i]);
        }
    }

    void VulkanDeviceQueue::Flush()
    {
        FlushCurrentCommandBuffer();
        PrepareNextCommandBuffer();
    }

    void VulkanDeviceQueue::WaitForGPU() const
    {
        auto queueWaitResult = m_queue.waitIdle();
        CHECK_VK_RESULT(queueWaitResult);
    }

    void VulkanDeviceQueue::FlushAndWait()
    {
        Flush();
        WaitForGPU();
    }

    void VulkanDeviceQueue::FlushCurrentCommandBuffer()
    {
        auto commandBufferEndResults = m_currentCommandBuffer->end();
        CHECK_VK_RESULT(commandBufferEndResults);

        vk::SubmitInfo submitInfo = {};

        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = m_currentCommandBuffer;
        
        if(IsCurrentSemaphore(EventType::BeginFrame))
        {
            submitInfo.waitSemaphoreCount = 1;
            submitInfo.pWaitSemaphores = m_currentSemaphores[static_cast<int>(EventType::BeginFrame)];
        }
        
        if(IsCurrentSemaphore(EventType::EndFrame))
        {
            submitInfo.signalSemaphoreCount = 1;
            submitInfo.pSignalSemaphores = m_currentSemaphores[static_cast<int>(EventType::EndFrame)];
        }

        Fence& fence = m_fences[m_currentCommandBufferIndex];

        auto submitResults = m_queue.submit(submitInfo, fence.fence);
        CHECK_VK_RESULT(submitResults);

        // mark signaled fence value
        fence.value = m_nextFenceValue;
        fence.active = true;

        // increment fence value
        m_nextFenceValue++;

        // No longer waiting on this semaphore
        CompleteSemaphore(EventType::BeginFrame);
        CompleteSemaphore(EventType::EndFrame);
    }

    void VulkanDeviceQueue::PrepareNextCommandBuffer()
    {
        m_currentCommandBufferIndex = (m_currentCommandBufferIndex + 1) % kCommandBufferCount;
        m_currentCommandBuffer = &m_commandBuffers[m_currentCommandBufferIndex];
        m_currentCommandPool = &m_commandPools[m_currentCommandBufferIndex];

        for (size_t i = 0; i < kCommandBufferCount; ++i)
        {
            UpdateFence(i, false);
        }

        UpdateFence(m_currentCommandBufferIndex, true);

        auto resetCommandPoolResults = m_device->resetCommandPool(*m_currentCommandPool);
        CHECK_VK_RESULT(resetCommandPoolResults);

        vk::CommandBufferBeginInfo beginInfo = {vk::CommandBufferUsageFlagBits::eOneTimeSubmit};
        auto commandBufferBeginResults = m_currentCommandBuffer->begin(beginInfo);
        CHECK_VK_RESULT(commandBufferBeginResults);
    }

    void VulkanDeviceQueue::SetCurrentSemaphore(EventType event)
    {
        pdlAssert(!IsCurrentSemaphore(event));
        auto* semaphore = GetSemaphore(event);
        m_currentSemaphores[static_cast<int>(event)] = semaphore;
    }

    void VulkanDeviceQueue::UpdateFence(size_t fenceIndex, bool wait)
    {
        Fence& fence = m_fences[fenceIndex];

        if (fence.active)
        {
            uint64_t timeout = wait ? ~static_cast<uint64_t>(0) : 0;

            auto waitFenceResult = m_device->waitForFences(1, &fence.fence, VK_TRUE, timeout);
            if(waitFenceResult == vk::Result::eSuccess)
            {
                auto resetFenceResult = m_device->resetFences(1, &fence.fence);
                CHECK_VK_RESULT(resetFenceResult);

                fence.active = false;

                m_lastFenceCompleted = Math::Max(fence.value, m_lastFenceCompleted);
            }
            else if(waitFenceResult != vk::Result::eTimeout)
            {
                CHECK_VK_RESULT(waitFenceResult);
            }
            
        }
    }
}

