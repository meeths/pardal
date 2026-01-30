
#pragma once
#include "VulkanBuffer.h"
#include "Base/BaseTypes.h"
#include "Renderer/RendererTypes.h"

// Created on 2026-01-30 by sisco

namespace pdl
{
class VulkanRenderer;

class VulkanImmediateCommand
{
public:
    struct Command
    {
        vk::CommandBuffer m_commandBuffer;
        SubmitHandle m_submitHandle;
        vk::Fence m_fence;
        vk::Semaphore m_semaphore;
        bool m_isAvailable = true;
    };
    
    static constexpr uint32 kMaxCommandBuffers = 64;
    
    
    VulkanImmediateCommand(VulkanRenderer& renderer, uint32_t queueFamilyIndex);
    ~VulkanImmediateCommand();
    
    const Command& Acquire(); 
    void Submit(Command& command);
    void Wait();
private:
    VulkanRenderer& m_renderer;
    vk::Queue m_queue;
    vk::CommandPool m_commandPool;
    uint32 m_queueFamilyIndex = 0;
    
    Command m_commands[kMaxCommandBuffers];
    
    SubmitHandle m_lastSubmitHandle = SubmitHandle();
    SubmitHandle m_nextSubmitHandle = SubmitHandle();

    uint32 m_availableCommands = kMaxCommandBuffers;
    
    vk::SemaphoreSubmitInfo m_lastSubmitSemaphore;
    vk::SemaphoreSubmitInfo m_waitSemaphore;
    vk::SemaphoreSubmitInfo m_signalSemaphore;

};

}

