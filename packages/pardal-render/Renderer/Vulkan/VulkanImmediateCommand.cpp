
#include <Renderer/Vulkan/VulkanImmediateCommand.h>

#include "VulkanRenderer.h"
#include "VulkanUtils.h"
#include "Containers/Array.h"

// Created on 2026-01-30 by sisco

namespace pdl
{
    VulkanImmediateCommand::VulkanImmediateCommand(VulkanRenderer& renderer, uint32_t queueFamilyIndex)
        : m_renderer(renderer), m_queueFamilyIndex(queueFamilyIndex)
    {
        auto& device = m_renderer.GetDevice();
        m_queue = device.getQueue(m_queueFamilyIndex, 0);
        
        vk::CommandPoolCreateInfo poolInfo(vk::CommandPoolCreateFlagBits::eResetCommandBuffer | vk::CommandPoolCreateFlagBits::eTransient, queueFamilyIndex);
        auto createCommandPoolResults = device.createCommandPool(poolInfo);
        CHECK_VK_RESULTVALUE(createCommandPoolResults);
        m_commandPool = createCommandPoolResults.value;
        
        vk::CommandBufferAllocateInfo allocInfo(m_commandPool, vk::CommandBufferLevel::ePrimary, 1);
        
        for (uint32 i = 0; i < kMaxCommandBuffers; ++i)
        {
            auto allocateCommandBufferResults = device.allocateCommandBuffers(allocInfo);
            CHECK_VK_RESULTVALUE(allocateCommandBufferResults);
            m_commands[i].m_commandBuffer = allocateCommandBufferResults.value[0];
            
            auto createFenceResults = device.createFence({vk::FenceCreateFlagBits::eSignaled});
            CHECK_VK_RESULTVALUE(createFenceResults);
            m_commands[i].m_fence = createFenceResults.value;
            
            auto createSemaphoreResults = device.createSemaphore({});
            CHECK_VK_RESULTVALUE(createSemaphoreResults);
            m_commands[i].m_semaphore = createSemaphoreResults.value;
        }
        
        m_waitSemaphore.stageMask = {vk::PipelineStageFlagBits2::eAllCommands};
        m_signalSemaphore.stageMask = {vk::PipelineStageFlagBits2::eAllCommands};
        m_lastSubmitSemaphore.stageMask = {vk::PipelineStageFlagBits2::eAllCommands};
        
    }

    VulkanImmediateCommand::~VulkanImmediateCommand()
    {
        Wait();
        
        for (auto& command : m_commands) 
        {
            m_renderer.GetDevice().destroyFence(command.m_fence);
            m_renderer.GetDevice().destroySemaphore(command.m_semaphore);       
        }

        m_renderer.GetDevice().destroyCommandPool(m_commandPool);
    }

    VulkanCommandBufferObject& VulkanImmediateCommand::Acquire()
    {   
        pdlAssert(m_availableCommands && "No available commands");
        
        VulkanCommandBufferObject* currentCommand = nullptr;
        for (VulkanCommandBufferObject& command : m_commands) 
        {
            if (command.m_isAvailable) 
            {
                currentCommand = &command;
                break;
            }
        }
        pdlAssert(currentCommand);
        auto waitForFenceResult = m_renderer.GetDevice().waitForFences({currentCommand->m_fence}, VK_TRUE, UINT64_MAX);
        CHECK_VK_RESULT(waitForFenceResult);
        
        auto resetCommandResult = currentCommand->m_commandBuffer.reset();
        CHECK_VK_RESULT(resetCommandResult);
        
        auto resetFenceResult = m_renderer.GetDevice().resetFences({currentCommand->m_fence});
        CHECK_VK_RESULT(resetFenceResult);
        
        auto beginResults = currentCommand->m_commandBuffer.begin({ vk::CommandBufferUsageFlagBits::eOneTimeSubmit });
        CHECK_VK_RESULT(beginResults);
        
        currentCommand->m_isAvailable = false;
        m_availableCommands--;
        return *currentCommand;
    }

    void VulkanImmediateCommand::Submit(VulkanCommandBufferObject& command)
    {
        pdlAssert(command.m_commandBuffer);
        pdlAssert(command.m_isAvailable == false);
        
        auto endResults = command.m_commandBuffer.end();
        CHECK_VK_RESULT(endResults);
        
        Array<vk::SemaphoreSubmitInfo, 2> semaphoreSubmitInfo;
        uint32 numWaitSemaphores = 0;
        
        if (m_waitSemaphore.semaphore)
        {
            semaphoreSubmitInfo[numWaitSemaphores++] = m_waitSemaphore;
        }
        if (m_lastSubmitSemaphore.semaphore)
        {
            semaphoreSubmitInfo[numWaitSemaphores++] = m_lastSubmitSemaphore;
        }
        
        vk::SemaphoreSubmitInfo signalSemaphore { command.m_semaphore, {}, vk::PipelineStageFlagBits2::eAllCommands};
        vk::CommandBufferSubmitInfo submitInfo { command.m_commandBuffer };
        vk::SubmitInfo2 submitInfo2 { {}, numWaitSemaphores, semaphoreSubmitInfo.data(), 1, &submitInfo, 1, &signalSemaphore  };
        auto submitResults = m_queue.submit2(1, &submitInfo2, command.m_fence);
        CHECK_VK_RESULT(submitResults);
        
        m_lastSubmitSemaphore.semaphore = command.m_semaphore;
        m_lastSubmitHandle = command.m_submitHandle;
        
        m_waitSemaphore.semaphore = nullptr;
        m_signalSemaphore.semaphore = nullptr;
        
        command.m_isAvailable = true;
        m_availableCommands++;
    }

    void VulkanImmediateCommand::Wait()
    {
        Array<vk::Fence, kMaxCommandBuffers> allFences;
        uint32 numFences = 0;
        
        for (auto& command : m_commands) 
        {
            if (!command.m_isAvailable)
            {
                allFences[numFences++] = command.m_fence;
            }
        }
        
        auto waitResults = m_renderer.GetDevice().waitForFences(numFences,allFences.data(), VK_TRUE, UINT64_MAX);
        CHECK_VK_RESULT(waitResults);
        
    }
}

