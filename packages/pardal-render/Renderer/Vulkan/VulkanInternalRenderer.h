
#pragma once
#include <Renderer/IInternalRenderer.h>

// Created on 2025-03-26 by sisco

namespace pdl
{
class VulkanDevice;

class VulkanInternalRenderer : public IInternalRenderer
{
public:
    VulkanInternalRenderer(VulkanDevice& device);
    ~VulkanInternalRenderer() override = default;
    
    bool BeginFrame() override;
    bool EndFrame() override;
    
    bool BeginRenderPass(const RenderPass& renderPass) override;
    bool EndRenderPass() override;

private:
    VulkanDevice& m_device;
    const RenderPass* m_lastRenderPass = nullptr;
    
};

}

