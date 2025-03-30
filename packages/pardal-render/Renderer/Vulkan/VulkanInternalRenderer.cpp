#include <Renderer/Vulkan/VulkanInternalRenderer.h>
#include <Renderer/Vulkan/VulkanDevice.h>
#include <Renderer/Vulkan/VulkanTextureView.h>

#include "VulkanTexture.h"
#include "VulkanUtils.h"
#include "Renderer/RenderPass.h"

// Created on 2025-03-26 by sisco

namespace pdl
{
    VulkanInternalRenderer::VulkanInternalRenderer(VulkanDevice& device) : m_device(device)
    {
    }

    bool VulkanInternalRenderer::BeginFrame()
    {
        m_device.GetVulkanDeviceQueue().PrepareNextCommandBuffer();
        m_device.GetVulkanDeviceQueue().SetCurrentSemaphore(VulkanDeviceQueue::EventType::BeginFrame);
        return true;
    }

    bool VulkanInternalRenderer::EndFrame()
    {
        for (auto& colorAttachment : m_lastRenderPass->GetColorAttachments())
        {
            VulkanTextureView* vulkanTextureView = static_cast<VulkanTextureView*>(colorAttachment);
            m_device.GetVulkanDeviceQueue().SetImageLayout(static_cast<VulkanTexture*>(vulkanTextureView->GetTexture()),
                                                           vk::ImageLayout::eUndefined,
                                                           vk::ImageLayout::ePresentSrcKHR);
        }
        m_device.GetVulkanDeviceQueue().SetCurrentSemaphore(VulkanDeviceQueue::EventType::EndFrame);
        m_device.GetVulkanDeviceQueue().FlushAndWait();
        m_lastRenderPass = nullptr;
        return true;
    }

    bool VulkanInternalRenderer::BeginRenderPass(const RenderPass& renderPass)
    {
        auto currentCommandBuffer = m_device.GetVulkanDeviceQueue().GetCommandBuffer();
        m_lastRenderPass = &renderPass;
        const auto& colorAttachments = renderPass.GetColorAttachments();
        const auto& colorClearValues = renderPass.GetClearColors();
        const auto depthAttachment = renderPass.GetDepthStencilAttachment();
        const auto depthClearValue = renderPass.GetDepthClearValue();
        const auto stencilclearValue = renderPass.GetStencilClearValue();

        Vector<vk::RenderingAttachmentInfoKHR> colorAttachmentInfos(colorAttachments.size());

        for (size_t i = 0; i < colorAttachments.size(); i++)
        {
            VulkanTextureView* vkTextureView = static_cast<VulkanTextureView*>(colorAttachments[i]);
            m_device.GetVulkanDeviceQueue().SetImageLayout(static_cast<VulkanTexture*>(vkTextureView->GetTexture()),
                                                           vk::ImageLayout::eUndefined,
                                                           vk::ImageLayout::eColorAttachmentOptimal);
            colorAttachmentInfos[i].imageView = vkTextureView->GetVkImageView();
            colorAttachmentInfos[i].imageLayout = vk::ImageLayout::eAttachmentOptimalKHR;
            colorAttachmentInfos[i].loadOp = vk::AttachmentLoadOp::eClear;
            colorAttachmentInfos[i].storeOp = vk::AttachmentStoreOp::eStore;
            auto clearColor = colorClearValues[i];
            colorAttachmentInfos[i].clearValue = vk::ClearValue{vk::ClearColorValue{clearColor.r, clearColor.g, clearColor.b, clearColor.a}};
        }

        vk::RenderingInfoKHR render_info;
        render_info.renderArea = {{{0, 0}, {1280, 720}}};
        render_info.layerCount = 1;
        render_info.colorAttachmentCount = (uint32)colorAttachmentInfos.size();
        render_info.pColorAttachments = colorAttachmentInfos.data();

        vk::RenderingAttachmentInfoKHR depthAttachmentInfo{};
        VulkanTextureView* vkDepthStencilView = static_cast<VulkanTextureView*>(depthAttachment);
        m_device.GetVulkanDeviceQueue().SetImageLayout((VulkanTexture*)vkDepthStencilView->GetTexture(),
                                                       vk::ImageLayout::eUndefined,
                                                       vk::ImageLayout::eDepthStencilAttachmentOptimal);
        depthAttachmentInfo.imageView = vkDepthStencilView->GetVkImageView();
        depthAttachmentInfo.imageLayout = vk::ImageLayout::eAttachmentOptimalKHR;
        depthAttachmentInfo.loadOp = vk::AttachmentLoadOp::eClear;
        depthAttachmentInfo.storeOp = vk::AttachmentStoreOp::eStore;
        depthAttachmentInfo.clearValue = vk::ClearValue{vk::ClearDepthStencilValue{depthClearValue, stencilclearValue}};
        render_info.pDepthAttachment = &depthAttachmentInfo;

        VULKAN_HPP_DEFAULT_DISPATCHER.vkCmdBeginRenderingKHR(currentCommandBuffer,
                                                             reinterpret_cast<const VkRenderingInfo*>(&render_info));

        return true;
    }

    bool VulkanInternalRenderer::EndRenderPass()
    {
        VULKAN_HPP_DEFAULT_DISPATCHER.vkCmdEndRenderingKHR(m_device.GetVulkanDeviceQueue().GetCommandBuffer());

        return true;
    }
}
