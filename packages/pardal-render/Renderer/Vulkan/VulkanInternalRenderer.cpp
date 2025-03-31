#include <Renderer/Vulkan/VulkanInternalRenderer.h>

#include <Renderer/RenderPass.h>
#include <Renderer/Vulkan/VulkanDevice.h>
#include <Renderer/Vulkan/VulkanSurface.h>
#include <Renderer/Vulkan/VulkanTexture.h>
#include <Renderer/Vulkan/VulkanTextureView.h>
#include <Renderer/Vulkan/VulkanUtils.h>
#include <Application/ApplicationWindow.h>

// Created on 2025-03-26 by sisco

namespace pdl
{
    VulkanInternalRenderer::VulkanInternalRenderer(VulkanDevice& device) : m_device(device)
    {
    }

    bool  VulkanInternalRenderer::Initialize(const InitInfo& initInfo)
    {
        auto surfaceResults = m_device.CreateSurface(initInfo.m_window);
        if(!surfaceResults)
        {
            pdlLogError("Could not create surface");
            return false;
        }
        m_surface = std::static_pointer_cast<VulkanSurface>(surfaceResults.value());

        pdl::ISurface::SwapchainDescriptor surfaceDescriptor
        {
            .m_format = initInfo.m_useHDR ? Format::R16G16B16A16_FLOAT : Format::R8G8B8A8_UNORM,     
            .m_size = initInfo.m_window.GetWindowSize(),
            .m_vsync= true
        };
    
        if (!m_surface->ConfigureSwapchain(surfaceDescriptor))
        {
            pdlLogError("Could not configure swapchain");
            return false;
        }

        if (initInfo.m_createDepthBuffer)
        {
            if (!BuildDepthBuffer(pdl::Format::D32_FLOAT, surfaceDescriptor.m_size))
            {
                pdlLogError("Could not create depth buffer");
                return false;
            }
        }

        return true;
    }

    bool VulkanInternalRenderer::BeginFrame()
    {
        if (!m_surface->BeginFrame())
        {
            pdlLogError("Could not begin frame");
            return false;
        }

        m_device.GetVulkanDeviceQueue().PrepareNextCommandBuffer();
        m_device.GetVulkanDeviceQueue().SetCurrentSemaphore(VulkanDeviceQueue::EventType::BeginFrame);

        Vector<ITextureView*> currentFrameSwapchainImageViews;
        Vector<Math::Vector4> clearColors;
        currentFrameSwapchainImageViews.push_back(m_surface->GetCurrentTextureView());
        clearColors.emplace_back(0, 0, 0, 0);
        m_mainRenderPass = RenderPass(currentFrameSwapchainImageViews, m_depthTextureView.get(), Math::Rectanglei({0,0}, m_surface->GetSurfaceConfig().m_size), clearColors);

        
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

        return m_surface->Present();
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
        const auto& renderArea = renderPass.GetRenderArea();
        render_info.renderArea = vk::Rect2D(
                vk::Offset2D((uint32)renderArea.GetOrigin().x, (uint32)renderArea.GetOrigin().y),
                vk::Extent2D((uint32)renderArea.GetDimensions().x, (uint32)renderArea.GetDimensions().y));
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

    void VulkanInternalRenderer::OnResize(Math::Vector2i newSize)
    {
        BuildDepthBuffer(pdl::Format::D32_FLOAT, newSize);
        auto swapchainDetails = m_surface->GetSurfaceConfig();
        swapchainDetails.m_size = newSize;
        m_surface->ConfigureSwapchain(swapchainDetails);
    }

    bool VulkanInternalRenderer::BuildDepthBuffer(Format format, Math::Vector2i size)
    {
        // Shared depth buffer
        ITexture::TextureDescriptor depthBufferDesc;
        depthBufferDesc.m_format = format;
        depthBufferDesc.m_extents.x = size.x;
        depthBufferDesc.m_extents.y = size.y;
        depthBufferDesc.m_extents.z = 1;
        depthBufferDesc.m_textureUsage = TextureUsage::DepthRead | TextureUsage::DepthWrite | TextureUsage::ShaderResource;

        auto depthBufferResult = m_device.CreateTexture(depthBufferDesc); 
        pdlAssert(depthBufferResult.has_value());
        m_depthTexture = std::static_pointer_cast<VulkanTexture>(depthBufferResult.value());
    
        ITextureView::TextureViewDescriptor depthStencilViewDesc;
        depthStencilViewDesc.m_texture = m_depthTexture.get();
        auto depthStencilViewResults = m_device.CreateTextureView(depthStencilViewDesc);
        pdlAssert(depthStencilViewResults.has_value());
        m_depthTextureView = std::static_pointer_cast<VulkanTextureView>(depthStencilViewResults.value());

        return true;
        
    }
}
