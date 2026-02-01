
#include <barrier>
#include <Renderer/Vulkan/VulkanCommandBuffer.h>

#include "VulkanUtils.h"
#include "Renderer/Vulkan/VulkanRenderer.h"

// Created on 2026-01-30 by sisco

namespace pdl
{
    VulkanCommandBuffer::VulkanCommandBuffer(VulkanRenderer* renderer, VulkanCommandBufferObject* command)
    : m_command(command), m_renderer(renderer)
    {
    }

    VulkanCommandBuffer::~VulkanCommandBuffer()
    {
        pdlAssert(!m_isRecording);
    }

    void VulkanCommandBuffer::BeginRendering(const RenderPass& renderPass, const Framebuffer& framebuffer,
                                             const Dependencies& dependencies)
    {
        pdlAssert(!m_isRecording);
        m_isRecording = true;
        
        // Handle dependencies
        for (auto& textureDependency : dependencies.m_textures)
        {
            if (textureDependency)
            {
                TransitionToShaderReadonly(textureDependency);
            }
        }
        for (auto& bufferDependency : dependencies.m_buffers)
        {
            if (bufferDependency)
            {
                vk::PipelineStageFlags2 stageFlags = vk::PipelineStageFlagBits2::eVertexShader | vk::PipelineStageFlagBits2::eFragmentShader;
                auto* buffer = m_renderer->Get(bufferDependency);
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
        }

        const uint32 numFbColorAttachments = framebuffer.GetNumColorAttachments();
        const uint32 numPassColorAttachments = renderPass.GetNumColorAttachments();
        pdlAssert(numPassColorAttachments == numFbColorAttachments);

        m_currentFramebuffer = framebuffer;
        
        for (const auto& colorAttachment : m_currentFramebuffer.m_colorAttachments) 
        {
            if (TextureHandle handle = colorAttachment.m_texture) 
            {
                auto* colorTexture = m_renderer->Get(handle);
                colorTexture->TransitionLayout(m_command->m_commandBuffer, vk::ImageLayout::eColorAttachmentOptimal);
            }
            // MSAA resolve
            if (TextureHandle handle = colorAttachment.m_resolveMSAATexture) 
            {
                auto* resolveTexture = m_renderer->Get(handle);
                resolveTexture->m_flags |= VulkanTexture::Flags::IsResolveAttachment;
                resolveTexture->TransitionLayout(m_command->m_commandBuffer, vk::ImageLayout::eColorAttachmentOptimal);
            }
        }

        
        if (TextureHandle depthHandle = m_currentFramebuffer.m_depthStencilTexture.m_texture) 
        {
            auto* depthTexture = m_renderer->Get(depthHandle);
            pdlAssert(depthTexture->m_vkFormat != vk::Format::eUndefined && "Invalid depth attachment format");
            pdlAssert(VulkanUtils::IsDepthFormat(depthTexture->m_vkFormat) && "Invalid depth attachment format");
            depthTexture->TransitionLayout(m_command->m_commandBuffer, vk::ImageLayout::eDepthStencilAttachmentOptimal);
        }
        if (TextureHandle depthResolveHandle = m_currentFramebuffer.m_depthStencilTexture.m_resolveMSAATexture) 
        {
            auto* depthResolveTexture = m_renderer->Get(depthResolveHandle);
            depthResolveTexture->m_flags |= VulkanTexture::Flags::IsResolveAttachment;
            pdlAssert(VulkanUtils::IsDepthFormat(depthResolveTexture->m_vkFormat) && "Invalid depth attachment format");
            depthResolveTexture->TransitionLayout(m_command->m_commandBuffer, vk::ImageLayout::eDepthStencilAttachmentOptimal);
        }

        // calculate and transition input attachments
        {
            uint32_t i = 0;
            while (i != RenderererConstants::MaxColorAttachments() && dependencies.m_inputAttachments[i]) 
            {
                TextureHandle handle = dependencies.m_inputAttachments[i];
                VulkanTexture* texture = m_renderer->Get(handle);
                texture->TransitionLayout(m_command->m_commandBuffer, vk::ImageLayout::eRenderingLocalReadKHR);
                m_currentInputAttachments.m_imageInfos[i] = 
                {
                    {},
                    texture->m_vkImageView,
                    vk::ImageLayout::eRenderingLocalReadKHR
                };
                m_currentInputAttachments.m_writes[i] = 
                {
                    {}, // ignored for push descriptors
                    i,
                    0u,
                    1u,
                    vk::DescriptorType::eInputAttachment,
                    &m_currentInputAttachments.m_imageInfos[i],
                };
                i++;
            }
            m_currentInputAttachments.m_count = i;
        }

        vk::SampleCountFlags samples = vk::SampleCountFlagBits::e1;
        uint32 mipLevel = 0;
        uint32 width = 0;
        uint32 height = 0;

        Array<vk::RenderingAttachmentInfo, RenderererConstants::MaxColorAttachments()> colorAttachments;

        for (uint32_t i = 0; i != numFbColorAttachments; i++)
        {
            auto& colorAttachment = framebuffer.m_colorAttachments[i];
            pdlAssert(colorAttachment.m_texture);

            auto* colorTexture = m_renderer->Get(colorAttachment.m_texture);
            
            auto& colorAttachmentDesc = renderPass.m_colorAttachments[i];
            if (mipLevel != 0 && colorAttachmentDesc.level != 0) 
            {
                pdlAssert(colorAttachmentDesc.level == mipLevel && "All color attachments should have the same mip-level");
            }
            auto dimensions = colorTexture->m_vkExtent;
            if (width != 0) 
            {
                pdlAssert(dimensions.width == width && "All attachments should have the same width");
            }
            if (height != 0) 
            {
                pdlAssert(dimensions.height == height && "All attachments should have the same height");
            }
            mipLevel = colorAttachmentDesc.level;
            width = dimensions.width;
            height = dimensions.height;
            samples = colorTexture->m_vkSamples;
            colorAttachments[i] = vk::RenderingAttachmentInfo
            { 
                colorTexture->GetOrCreateVkImageViewForFramebuffer(*m_renderer, colorAttachmentDesc.level, colorAttachmentDesc.layer),
                colorTexture->m_vkLayout,
                (samples > vk::SampleCountFlagBits::e1) ? 
                    VulkanUtils::GetResolveModeFlag(colorAttachmentDesc.resolveMode)
                    : vk::ResolveModeFlagBits::eNone,
                {},
                vk::ImageLayout::eUndefined,
                VulkanUtils::GetAttachmentLoadOp(colorAttachmentDesc.loadOp),
                VulkanUtils::GetAttachmentStoreOp(colorAttachmentDesc.storeOp),
            };
            colorAttachments[i].clearValue.color = 
            {
                colorAttachmentDesc.clearColor.x, 
                colorAttachmentDesc.clearColor.y, 
                colorAttachmentDesc.clearColor.z, 
                colorAttachmentDesc.clearColor.w
            };
            // handle MSAA
            if (colorAttachmentDesc.storeOp == StoreOp::MsaaResolve) 
            {
                pdlAssert(samples > vk::SampleCountFlagBits::e1);
                pdlAssert(colorAttachment.m_resolveMSAATexture && "Framebuffer attachment should contain a resolve texture");
                auto* colorResolveTexture = m_renderer->Get(colorAttachment.m_resolveMSAATexture);
                colorAttachments[i].resolveImageView =
                    colorResolveTexture->GetOrCreateVkImageViewForFramebuffer(*m_renderer, colorAttachmentDesc.level, colorAttachmentDesc.layer);
                colorAttachments[i].resolveImageLayout = vk::ImageLayout::eColorAttachmentOptimal;
            }

        }
        
        vk::RenderingInfo renderingInfo
        {
            {},
            {{0, 0}, {width, height}},
            1, 
            0,
            numFbColorAttachments,
            colorAttachments.data()
        };
        m_command->m_commandBuffer.beginRendering(renderingInfo);
    }

    void VulkanCommandBuffer::EndRendering()
    {
        pdlAssert(m_isRecording);
        m_isRecording = false;
        m_command->m_commandBuffer.endRendering();
    }

    void VulkanCommandBuffer::TransitionToShaderReadonly(TextureHandle textureHandle)
    {
        auto* texture = m_renderer->Get(textureHandle);
        pdlAssert (texture->m_vkSamples == vk::SampleCountFlagBits::e1);
        texture->TransitionLayout(m_command->m_commandBuffer, vk::ImageLayout::eShaderReadOnlyOptimal);
        
    }
    
    void VulkanCommandBuffer::BufferBarrier(BufferHandle bufferHandle, vk::PipelineStageFlags2 srcStage, vk::PipelineStageFlags2 dstStage)
    {
        auto* buffer = m_renderer->Get(bufferHandle);
        
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
        
        m_command->m_commandBuffer.pipelineBarrier2(dependencyInfo);
    }
}

