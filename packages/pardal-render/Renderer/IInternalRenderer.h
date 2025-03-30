
#pragma once
#include <Containers/Vector.h>

#include "Math/Vector4.h"

// Created on 2025-03-26 by sisco


namespace pdl
{
class RenderPass;
class ITextureView;

class IInternalRenderer
{
public:
    virtual ~IInternalRenderer() = default;

    virtual bool BeginFrame() = 0;
    virtual bool EndFrame() = 0;

    virtual bool BeginRenderPass(const RenderPass& renderPass) = 0;
    virtual bool EndRenderPass() = 0;
};

}

