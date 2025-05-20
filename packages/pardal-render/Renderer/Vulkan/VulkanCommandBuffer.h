
#pragma once
#include <Renderer/ICommandBuffer.h>
#include <vulkan/vulkan.hpp>

// Created on 2025-05-10 by sisco

namespace pdl
{

class VulkanCommandBuffer : public ICommandBuffer
{
public:
    VulkanCommandBuffer() = default;
    VulkanCommandBuffer(vk::CommandBuffer commandBuffer) : m_commandBuffer(commandBuffer) {}
    const vk::CommandBuffer& GetVkCommandBuffer() const { return m_commandBuffer; }
private:
    vk::CommandBuffer m_commandBuffer = VK_NULL_HANDLE;
};

}

