
#include <Renderer/RenderPass.h>
#include <utility>
#include <Base/DebugHelpers.h>

// Created on 2025-03-30 by sisco

namespace pdl
{
    RenderPass::RenderPass(Vector<ITextureView*> colorAttachments, ITextureView* depthStencilAttachment)
        : m_colorAttachments(std::move(colorAttachments)), m_depthStencilAttachment(depthStencilAttachment)
    {
        m_clearColors.resize(m_colorAttachments.size(), Math::Vector4(0.0f, 0.0f, 0.0f, 0.0f));
    }

    RenderPass::RenderPass(Vector<ITextureView*> colorAttachments, ITextureView* depthStencilAttachment,
        Vector<Math::Vector4> clearColors, float depthClearValue, uint16 stencilClearValue) :
        RenderPass(std::move(colorAttachments), depthStencilAttachment)
    {
        m_clearColors = std::move(clearColors);
        m_depthClearValue = depthClearValue;
        m_stencilClearValue = stencilClearValue;

        pdlAssert(m_clearColors.size() == m_colorAttachments.size());
    }
}

