
#pragma once
#include <Base/BaseTypes.h>
#include <Containers/Vector.h>
#include <Math/Rectangle.h>
#include <Math/Vector4.h>

// Created on 2025-03-30 by sisco

namespace pdl
{
class ITextureView;

class RenderPass
{
public:
    RenderPass() = default;
    RenderPass(Vector<ITextureView*> colorAttachments, ITextureView* depthStencilAttachment, Math::Rectanglei renderArea);
    RenderPass(Vector<ITextureView*> colorAttachments, ITextureView* depthStencilAttachment, Math::Rectanglei renderArea, Vector<Math::Vector4> clearColors, float depthClearValue = 1.0f, uint16 stencilClearValue = 0);
    ~RenderPass() = default;

    const Vector<ITextureView*>& GetColorAttachments() const { return m_colorAttachments; }
    ITextureView* GetDepthStencilAttachment() const { return m_depthStencilAttachment; }
    const Vector<Math::Vector4>& GetClearColors() const { return m_clearColors; }
    float GetDepthClearValue() const { return m_depthClearValue; }
    uint16 GetStencilClearValue() const { return m_stencilClearValue; }
    const Math::Rectanglei& GetRenderArea() const { return m_renderArea; }

    void SetClearColor(uint32 index, Math::Vector4 color) { m_clearColors[index] = color; }
private:
    Vector<ITextureView*> m_colorAttachments;
    Vector<Math::Vector4> m_clearColors;
    ITextureView* m_depthStencilAttachment = nullptr;
    Math::Rectanglei m_renderArea;
    float m_depthClearValue = 1.0f;
    uint16 m_stencilClearValue = 0;
};

}

