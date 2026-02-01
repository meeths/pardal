
#pragma once
#include "RendererTypes.h"
// Created on 2026-01-30 by sisco

namespace pdl
{

class ICommandBuffer
{
public:
    virtual ~ICommandBuffer() = default;
    
    virtual void BeginRendering(const RenderPass& renderPass, const Framebuffer& desc, const Dependencies& dependencies) = 0;
    virtual void EndRendering() = 0;
    virtual bool IsRecording() const = 0;
    
    virtual void TransitionToShaderReadonly(TextureHandle texture) = 0;

    
};

}

