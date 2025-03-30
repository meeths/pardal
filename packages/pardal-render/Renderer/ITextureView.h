
#pragma once
#include <Base/BaseTypes.h>

// Created on 2025-03-26 by sisco

namespace pdl
{
class ITexture;
    
class ITextureView
{
public:
    struct TextureViewDescriptor
    {
        ITexture* m_texture;
        uint32 m_baseMipLevel = 0;
    };

    virtual ~ITextureView() = default;
};

}

