
#pragma once
#include "Renderer/ICommandBuffer.h"
#include "Renderer/Vulkan/VulkanCommandBufferObject.h"

// Created on 2026-01-30 by sisco

namespace pdl
{
class VulkanRenderer;

class VulkanCommandBuffer : public ICommandBuffer
{
public:
    VulkanCommandBuffer(VulkanRenderer& renderer, VulkanCommandBufferObject& command);
    ~VulkanCommandBuffer() override;

    void BeginRecording(const RenderPass& renderPass, const Framebuffer& desc, const Dependencies& dependencies) override;
    void EndRecording() override;
    bool IsRecording() const override { return m_isRecording; }
    
    void TransitionToShaderReadonly(TextureHandle textureHandle) override;

private:
    
    void BufferBarrier(BufferHandle bufferHandle, vk::PipelineStageFlags2 srcStage, vk::PipelineStageFlags2 dstStage);
    
    VulkanRenderer& m_renderer;
    VulkanCommandBufferObject& m_command;
    bool m_isRecording = false;
};

}

