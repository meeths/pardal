
#include <barrier>
#include <Renderer/Vulkan/VulkanCommandBuffer.h>
#include "Renderer/Vulkan/VulkanRenderer.h"

// Created on 2026-01-30 by sisco

namespace pdl
{
    VulkanCommandBuffer::VulkanCommandBuffer(VulkanRenderer& renderer, VulkanCommandBufferObject& command)
    : m_renderer(renderer), m_command(command)
    {
    }

    void VulkanCommandBuffer::BeginRecording(const RenderPass& renderPass, const Framebuffer& desc,
                                             const Dependencies& dependencies)
    {
        pdlAssert(!m_isRecording);
        m_isRecording = true;
        
        // Handle dependencies
        for (auto& textureDependency : dependencies.m_textures)
        {
            TransitionToShaderReadonly(textureDependency);
        }
        for (auto& bufferDependency : dependencies.m_buffers)
        {
            vk::PipelineStageFlags2 stageFlags = vk::PipelineStageFlagBits2::eVertexShader | vk::PipelineStageFlagBits2::eFragmentShader;
            auto* buffer = m_renderer.Get(bufferDependency);
            if (buffer->m_usage & vk::BufferUsageFlagBits::eIndexBuffer || buffer->m_usage & vk::BufferUsageFlagBits::eVertexBuffer)
            {
                stageFlags |= vk::PipelineStageFlagBits2::eVertexInput;
            }
            if (buffer->m_usage & vk::BufferUsageFlagBits::eIndirectBuffer)
            {
                stageFlags |= vk::PipelineStageFlagBits2::eDrawIndirect;
            }
            BufferBarrier(bufferDependency, stageFlags, stageFlags);
        }

        //
        
        vk::RenderingInfo renderingInfo
        {
            
        };
        m_command.m_commandBuffer.beginRendering(renderingInfo);
    }

    void VulkanCommandBuffer::EndRecording()
    {
        pdlAssert(m_isRecording);
        m_isRecording = false;
        m_command.m_commandBuffer.endRendering();
    }

    void VulkanCommandBuffer::TransitionToShaderReadonly(TextureHandle textureHandle)
    {
        auto* texture = m_renderer.Get(textureHandle);
        pdlAssert (texture->m_vkSamples == vk::SampleCountFlagBits::e1);
        texture->TransitionLayout(m_command.m_commandBuffer, vk::ImageLayout::eShaderReadOnlyOptimal);
        
    }

    void VulkanCommandBuffer::BufferBarrier(BufferHandle bufferHandle, vk::PipelineStageFlags2 srcStage, vk::PipelineStageFlags2 dstStage)
    {
        auto* buffer = m_renderer.Get(bufferHandle);
        
        vk::BufferMemoryBarrier2 bufferMemoryBarrier
        {
            srcStage,
            {},
            dstStage,
            {},
            vk::QueueFamilyIgnored,
            vk::QueueFamilyIgnored,
            buffer->m_buffer,
            0,
            vk::WholeSize
        };
     
        if (srcStage & vk::PipelineStageFlagBits2::eTransfer)
        {
            bufferMemoryBarrier.srcAccessMask |= vk::AccessFlagBits2::eTransferWrite | vk::AccessFlagBits2::eTransferRead;
        }
        else
        {
            bufferMemoryBarrier.srcAccessMask |= vk::AccessFlagBits2::eShaderWrite | vk::AccessFlagBits2::eShaderRead;
        }
        
        if (dstStage & vk::PipelineStageFlagBits2::eTransfer)
        {
            bufferMemoryBarrier.dstAccessMask |= vk::AccessFlagBits2::eTransferWrite | vk::AccessFlagBits2::eTransferRead;
        }
        else
        {
            bufferMemoryBarrier.dstAccessMask |= vk::AccessFlagBits2::eShaderWrite | vk::AccessFlagBits2::eShaderRead;
        }
        
        if (dstStage & vk::PipelineStageFlagBits2::eDrawIndirect)
        {
            bufferMemoryBarrier.dstAccessMask |= vk::AccessFlagBits2::eIndirectCommandRead;       
        }
        
        if (buffer->m_usage & vk::BufferUsageFlagBits::eIndexBuffer)
        {
            bufferMemoryBarrier.dstAccessMask |= vk::AccessFlagBits2::eIndexRead;    
        }
        
        vk::DependencyInfo dependencyInfo
        {
            {},0, nullptr,
            1, &bufferMemoryBarrier
        };
        
        m_command.m_commandBuffer.pipelineBarrier2(dependencyInfo);
    }
}

