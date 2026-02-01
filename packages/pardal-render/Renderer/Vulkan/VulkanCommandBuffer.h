
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
    VulkanCommandBuffer() = default;
    VulkanCommandBuffer(VulkanRenderer* renderer, VulkanCommandBufferObject* command);
    ~VulkanCommandBuffer() override;

    void BeginRendering(const RenderPass& renderPass, const Framebuffer& framebuffer, const Dependencies& dependencies) override;
    void EndRendering() override;
    bool IsRecording() const override { return m_isRecording; }
    
    void TransitionToShaderReadonly(TextureHandle textureHandle) override;

    VulkanCommandBufferObject* m_command = nullptr;

private:
    void BufferBarrier(BufferHandle bufferHandle, vk::PipelineStageFlags2 srcStage, vk::PipelineStageFlags2 dstStage);
    
    VulkanRenderer* m_renderer = nullptr;;
//    VulkanCommandBufferObject* m_command = nullptr;
    bool m_isRecording = false;
    
    Framebuffer m_currentFramebuffer;
    struct 
    {
        Array<vk::DescriptorImageInfo, RenderererConstants::MaxColorAttachments()> m_imageInfos;
        Array<vk::WriteDescriptorSet, RenderererConstants::MaxColorAttachments()> m_writes;
        uint32_t m_count = 0;
    } m_currentInputAttachments;

};

}

