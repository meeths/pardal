
#include "Renderer/Vulkan/VulkanTexture.h"

#include "Renderer/Vulkan/VulkanUtils.h"
#include "Renderer/Vulkan/VulkanRenderer.h"

// Created on 2026-01-30 by sisco
namespace Details
{
    struct PipelineStageAccess
    {
        vk::PipelineStageFlags2 stage;
        vk::AccessFlags2 access;
    };
    
    PipelineStageAccess GetPipelineStageAccess(vk::ImageLayout imageLayout)
    {
        PipelineStageAccess pipelineStageAccess;
        switch (imageLayout) {
        case vk::ImageLayout::eUndefined:
            pipelineStageAccess.stage = vk::PipelineStageFlagBits2::eTopOfPipe;
            pipelineStageAccess.access = vk::AccessFlagBits2::eNone;
            break;
        case vk::ImageLayout::eGeneral:
            pipelineStageAccess.stage = vk::PipelineStageFlagBits2::eComputeShader | vk::PipelineStageFlagBits2::eTransfer;
            pipelineStageAccess.access = vk::AccessFlagBits2::eMemoryRead | vk::AccessFlagBits2::eMemoryWrite | vk::AccessFlagBits2::eTransferWrite;
            break;
        case vk::ImageLayout::eColorAttachmentOptimal:
            pipelineStageAccess.stage = vk::PipelineStageFlagBits2::eColorAttachmentOutput;
            pipelineStageAccess.access = vk::AccessFlagBits2::eColorAttachmentWrite | vk::AccessFlagBits2::eColorAttachmentRead;
            break;
        case vk::ImageLayout::eDepthStencilAttachmentOptimal:
            pipelineStageAccess.stage = vk::PipelineStageFlagBits2::eEarlyFragmentTests | vk::PipelineStageFlagBits2::eLateFragmentTests;
            pipelineStageAccess.access = vk::AccessFlagBits2::eDepthStencilAttachmentWrite | vk::AccessFlagBits2::eDepthStencilAttachmentRead;
            break;
        case vk::ImageLayout::eShaderReadOnlyOptimal:
            pipelineStageAccess.stage = vk::PipelineStageFlagBits2::eFragmentShader | vk::PipelineStageFlagBits2::eComputeShader| vk::PipelineStageFlagBits2::ePreRasterizationShaders;
            pipelineStageAccess.access = vk::AccessFlagBits2::eShaderRead;
            break;
        case vk::ImageLayout::eTransferSrcOptimal:
            pipelineStageAccess.stage = vk::PipelineStageFlagBits2::eTransfer;
            pipelineStageAccess.access = vk::AccessFlagBits2::eTransferRead;
            break;
        case vk::ImageLayout::eTransferDstOptimal:
            pipelineStageAccess.stage = vk::PipelineStageFlagBits2::eTransfer;
            pipelineStageAccess.access = vk::AccessFlagBits2::eTransferWrite;
            break;
        case vk::ImageLayout::ePresentSrcKHR:
            pipelineStageAccess.stage = vk::PipelineStageFlagBits2::eColorAttachmentOutput | vk::PipelineStageFlagBits2::eComputeShader;
            pipelineStageAccess.access = vk::AccessFlagBits2::eShaderWrite;
        case vk::ImageLayout::eRenderingLocalReadKHR:
            pipelineStageAccess.stage = vk::PipelineStageFlagBits2::eColorAttachmentOutput | vk::PipelineStageFlagBits2::eFragmentShader;
            pipelineStageAccess.access = vk::AccessFlagBits2::eColorAttachmentRead | vk::AccessFlagBits2::eColorAttachmentWrite | vk::AccessFlagBits2::eInputAttachmentRead;
            break;
        default:
            pdlLogWarning("Unsupported image layout stage access: %d", static_cast<int>(imageLayout));
            pipelineStageAccess.stage = vk::PipelineStageFlagBits2::eAllCommands;
            pipelineStageAccess.access = vk::AccessFlagBits2::eMemoryRead | vk::AccessFlagBits2::eMemoryWrite;
        }
        return pipelineStageAccess;
    }
}
namespace pdl
{
    vk::ImageView VulkanTexture::CreateView(VulkanRenderer& renderer, vk::ImageViewType viewType, vk::Format format,
        vk::ImageAspectFlags aspectFlags, uint32 baseMip, uint32 numMips, uint32 baseLayer, uint32 numLayers,
        vk::ComponentMapping componentMapping) const
    {
        vk::ImageViewCreateInfo createInfo(
            {},
            m_vkImage,
            viewType,
            format,
            componentMapping,
            vk::ImageSubresourceRange(aspectFlags, baseMip, numMips, baseLayer, numLayers));
        
        auto createImageViewResults = renderer.GetDevice().createImageView(createInfo);
        CHECK_VK_RESULTVALUE(createImageViewResults);
        return createImageViewResults.value;
    }

    vk::ImageAspectFlags VulkanTexture::GetAspectFlags() const
    {
        vk::ImageAspectFlags aspectFlags = {};
        if (VulkanUtils::IsDepthFormat(m_vkFormat)) aspectFlags |= vk::ImageAspectFlagBits::eDepth;
        if (VulkanUtils::IsStencilFormat(m_vkFormat)) aspectFlags |= vk::ImageAspectFlagBits::eStencil;
        if (aspectFlags == vk::ImageAspectFlags()) aspectFlags |= vk::ImageAspectFlagBits::eColor;
        return aspectFlags;
    }

    void VulkanTexture::TransitionLayout(vk::CommandBuffer commandBuffer, vk::ImageLayout newLayout)
    {
        vk::ImageLayout oldLayout;
        const bool isDepthAttachment = !!(m_vkUsage & vk::ImageUsageFlagBits::eDepthStencilAttachment);
        
        if (m_vkLayout == vk::ImageLayout::eAttachmentOptimal)
        {
            if (isDepthAttachment)
                oldLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal;
            else
                oldLayout = vk::ImageLayout::eColorAttachmentOptimal;
        }
        else
        {
            oldLayout = m_vkLayout;       
        }
        
        if (oldLayout == newLayout)
            return;
        
        Details::PipelineStageAccess dstPipelineStageAccess = Details::GetPipelineStageAccess(newLayout);
        Details::PipelineStageAccess srcPipelineStageAccess = Details::GetPipelineStageAccess(oldLayout);
        
        bool isResolveAttachment = !!(m_flags & Flags::IsResolveAttachment); 
        if (isResolveAttachment && isDepthAttachment)
        {
            srcPipelineStageAccess.stage |= vk::PipelineStageFlagBits2::eColorAttachmentOutput;
            srcPipelineStageAccess.access |= vk::AccessFlagBits2::eColorAttachmentRead | vk::AccessFlagBits2::eColorAttachmentWrite;       
            dstPipelineStageAccess.stage |= vk::PipelineStageFlagBits2::eColorAttachmentOutput;
            dstPipelineStageAccess.access |= vk::AccessFlagBits2::eColorAttachmentRead | vk::AccessFlagBits2::eColorAttachmentWrite;       
        }
        
        const vk::ImageMemoryBarrier2 imageBarrier 
        {
            srcPipelineStageAccess.stage,
            srcPipelineStageAccess.access,
            dstPipelineStageAccess.stage,
            dstPipelineStageAccess.access,
            oldLayout,
            newLayout,
            vk::QueueFamilyIgnored,
            vk::QueueFamilyIgnored,
            m_vkImage,
            vk::ImageSubresourceRange(GetAspectFlags(), 0, 1, 0, 1)
        };
        
        const vk::DependencyInfo dependencyInfo
        {
            {},0,nullptr, 0, nullptr,
            1, &imageBarrier
        };
        

        commandBuffer.pipelineBarrier2(dependencyInfo);
        
        m_vkLayout = newLayout;
    }
}

