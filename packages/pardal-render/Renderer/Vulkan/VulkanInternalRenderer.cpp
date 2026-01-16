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
        m_surface = eastl::static_pointer_cast<VulkanSurface>(surfaceResults.value());

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

//        if (!m_pipelineStateInitialized)
        {
//          m_pipelineStateInitialized = true;
            SetPipelineState(GetCommandBuffer(), m_currentPipelineState, true);       
        }

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

        VULKAN_HPP_DEFAULT_DISPATCHER.vkCmdBeginRenderingKHR(currentCommandBuffer->GetVkCommandBuffer(),
                                                             reinterpret_cast<const VkRenderingInfo*>(&render_info));

        return true;
    }

    bool VulkanInternalRenderer::EndRenderPass()
    {
        VULKAN_HPP_DEFAULT_DISPATCHER.vkCmdEndRenderingKHR(m_device.GetVulkanDeviceQueue().GetCommandBuffer()->GetVkCommandBuffer());

        return true;
    }

    void VulkanInternalRenderer::OnResize(Math::Vector2i newSize)
    {
        BuildDepthBuffer(pdl::Format::D32_FLOAT, newSize);
        auto swapchainDetails = m_surface->GetSurfaceConfig();
        swapchainDetails.m_size = newSize;
        m_surface->ConfigureSwapchain(swapchainDetails);
    }

    void VulkanInternalRenderer::SetPipelineState(ICommandBuffer* cmd, const PipelineState& pipelineState, bool force)
    {
        auto vkcmd = static_cast<VulkanCommandBuffer*>(GetCommandBuffer())->GetVkCommandBuffer();

        if (pipelineState.m_blendMode != m_currentPipelineState.m_blendMode || force)
        {
            const vk::Bool32 blendEnable = pipelineState.m_blendMode.m_enabled ? VK_TRUE : VK_FALSE;
            const auto blendEquation = VulkanUtils::GetBlendEquation(pipelineState.m_blendMode.m_equation);
            vkcmd.setColorBlendEnableEXT(0, m_lastRenderPass ? m_lastRenderPass->GetColorAttachments().size() : 1, &blendEnable);
            vkcmd.setColorBlendEquationEXT(0, m_lastRenderPass ? m_lastRenderPass->GetColorAttachments().size() : 1, &blendEquation);
        }

        if (pipelineState.m_cullMode != m_currentPipelineState.m_cullMode || force)
        {
            vkcmd.setCullModeEXT(VulkanUtils::GetCullMode(pipelineState.m_cullMode));
        }

        if (pipelineState.m_depthTest != m_currentPipelineState.m_depthTest || force)
        {
            vkcmd.setDepthTestEnableEXT(pipelineState.m_depthTest.m_enabled ? VK_TRUE : VK_FALSE);
            vkcmd.setDepthCompareOpEXT(VulkanUtils::GetCompareOp(pipelineState.m_depthTest.m_compareOp));
            vkcmd.setDepthWriteEnableEXT(pipelineState.m_depthTest.m_writeEnabled ? VK_TRUE : VK_FALSE);
        }

        
        if (pipelineState.m_depthBias != m_currentPipelineState.m_depthBias || force)
        {
            vkcmd.setDepthBiasEnable(pipelineState.m_depthBias.m_enabled ? VK_TRUE : VK_FALSE);
            vkcmd.setDepthBias(pipelineState.m_depthBias.m_constantFactor,
                pipelineState.m_depthBias.m_clamp,
                pipelineState.m_depthBias.m_slopeFactor);
        }

        if (pipelineState.m_frontFace != m_currentPipelineState.m_frontFace || force)
        {
            vkcmd.setFrontFaceEXT(VulkanUtils::GetFrontFace(pipelineState.m_frontFace));
        }

        if (pipelineState.m_polygonMode != m_currentPipelineState.m_polygonMode || force)
        {
            vkcmd.setPolygonModeEXT(VulkanUtils::GetPolygonMode(pipelineState.m_polygonMode));
        }

        if (pipelineState.m_stencilTest != m_currentPipelineState.m_stencilTest || force)
        {
            vkcmd.setStencilTestEnableEXT(pipelineState.m_stencilTest.m_enabled ? VK_TRUE : VK_FALSE);
            vkcmd.setStencilOp(
                VulkanUtils::GetStencilFaceFlags(pipelineState.m_stencilTest.m_face),
                VulkanUtils::GetStencilOp(pipelineState.m_stencilTest.m_failOp),
                VulkanUtils::GetStencilOp(pipelineState.m_stencilTest.m_passOp),
                VulkanUtils::GetStencilOp(pipelineState.m_stencilTest.m_depthFailOp),
                VulkanUtils::GetCompareOp(pipelineState.m_stencilTest.m_compareOp));
        }

        vkcmd.setPrimitiveRestartEnable(VK_FALSE);

        if (pipelineState.m_topology != m_currentPipelineState.m_topology || force)
        {
            vkcmd.setPrimitiveTopologyEXT(VulkanUtils::GetPrimitiveTopology(pipelineState.m_topology));
        }

        if (pipelineState.m_rasterizerDiscard != m_currentPipelineState.m_rasterizerDiscard || force)
        {
            vkcmd.setRasterizerDiscardEnable(pipelineState.m_rasterizerDiscard ? VK_TRUE : VK_FALSE);
        }

        if (pipelineState.m_rasterSamples != m_currentPipelineState.m_rasterSamples || force)
        {
            vkcmd.setRasterizationSamplesEXT(VulkanUtils::GetMultisampleFlagBits(pipelineState.m_rasterSamples));
        }
        if (pipelineState.m_rasterSampleMask != m_currentPipelineState.m_rasterSampleMask || force)
        {
            vkcmd.setSampleMaskEXT(VulkanUtils::GetMultisampleFlagBits(pipelineState.m_rasterSamples), &pipelineState.m_rasterSampleMask);
        }
        if (pipelineState.m_conservativeRasterization != m_currentPipelineState.m_conservativeRasterization || force)
        {
            vkcmd.setConservativeRasterizationModeEXT(VulkanUtils::GetConservativeRasterizationMode(pipelineState.m_conservativeRasterization));
        }

        if (pipelineState.m_alphaToCoverage != m_currentPipelineState.m_alphaToCoverage || force)
        {
            vkcmd.setAlphaToCoverageEnableEXT(pipelineState.m_alphaToCoverage ? VK_TRUE : VK_FALSE);
        }
        
        if (pipelineState.m_colorWriteMask != m_currentPipelineState.m_colorWriteMask || force)
        {
            vk::ColorComponentFlags colorCompFlags = VulkanUtils::GetColorComponentMasks(pipelineState.m_colorWriteMask);  
            vkcmd.setColorWriteMaskEXT(0, m_lastRenderPass ? m_lastRenderPass->GetColorAttachments().size() : 1, &colorCompFlags);
        }

        vkcmd.setVertexInputEXT(0, nullptr, 0, nullptr);
        
        m_currentPipelineState = pipelineState;
    }

    void VulkanInternalRenderer::SetViewport(ICommandBuffer* cmd, const Math::Rectangle& viewport)
    {
        auto vkcmd = static_cast<VulkanCommandBuffer*>(GetCommandBuffer())->GetVkCommandBuffer();
        const vk::Viewport vkviewport = {viewport.mMin.x, viewport.mMin.y, viewport.mMax.x, viewport.mMax.y};
        vkcmd.setViewportWithCount(1, &vkviewport);
    }

    void VulkanInternalRenderer::SetScissor(ICommandBuffer* cmd, const Math::Rectanglei& scissor)
    {
        auto vkcmd = static_cast<VulkanCommandBuffer*>(GetCommandBuffer())->GetVkCommandBuffer();
        vk::Rect2D vkscissor = {{scissor.mMin.x, scissor.mMin.y}, {(uint32)scissor.mMax.x, (uint32)scissor.mMax.y}};
        vkcmd.setScissorWithCount(1, &vkscissor);
    }

    ICommandBuffer* VulkanInternalRenderer::GetCommandBuffer()
    {
        return m_device.GetVulkanDeviceQueue().GetCommandBuffer();
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
        m_depthTexture = eastl::static_pointer_cast<VulkanTexture>(depthBufferResult.value());
    
        ITextureView::TextureViewDescriptor depthStencilViewDesc;
        depthStencilViewDesc.m_texture = m_depthTexture.get();
        auto depthStencilViewResults = m_device.CreateTextureView(depthStencilViewDesc);
        pdlAssert(depthStencilViewResults.has_value());
        m_depthTextureView = eastl::static_pointer_cast<VulkanTextureView>(depthStencilViewResults.value());

        return true;
        
    }
}
