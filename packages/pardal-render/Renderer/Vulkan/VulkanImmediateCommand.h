
#pragma once
#include "VulkanCommandBufferObject.h"
#include "Base/BaseTypes.h"
#include "Renderer/RendererTypes.h"

// Created on 2026-01-30 by sisco

namespace pdl
{
class VulkanRenderer;

class VulkanImmediateCommand
{
public:
    
    static constexpr uint32 kMaxCommandBuffers = 64;
    
    
    VulkanImmediateCommand(VulkanRenderer& renderer, uint32_t queueFamilyIndex);
    ~VulkanImmediateCommand();
    
    VulkanCommandBufferObject& Acquire(); 
    vk::Semaphore AcquireLastSubmitSemaphore() 
    {
        return std::exchange(m_lastSubmitSemaphore.semaphore, {});
    }

    void Submit(VulkanCommandBufferObject& command);
    void Wait();
private:
    VulkanRenderer& m_renderer;
    vk::Queue m_queue;
    vk::CommandPool m_commandPool;
    uint32 m_queueFamilyIndex = 0;
    
    VulkanCommandBufferObject m_commands[kMaxCommandBuffers];
    
    SubmitHandle m_lastSubmitHandle = SubmitHandle();
    SubmitHandle m_nextSubmitHandle = SubmitHandle();

    uint32 m_availableCommands = kMaxCommandBuffers;
    
    vk::SemaphoreSubmitInfo m_lastSubmitSemaphore;
    vk::SemaphoreSubmitInfo m_waitSemaphore;
    vk::SemaphoreSubmitInfo m_signalSemaphore;

};

}

