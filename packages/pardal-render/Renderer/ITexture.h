#pragma once
#include <Base/BaseTypes.h>
#include <Math/Vector3i.h>
#include <Renderer/RendererTypes.h>

// Created on 2025-03-23 by sisco

namespace pdl
{

class ITexture
{
public:
    struct SampleDesc
    {
        int32 m_numSamples = 1;
        int32 m_quality = 0;
    };

    struct Descriptor
    {
        Math::Vector3i m_extents;
        int32 m_arraySize = 0;
        int m_mipLevels = 0;
        Format m_format = Format::Unknown;
        SampleDesc m_sampleDesc;
        ResourceState m_state = ResourceState::Undefined;
    };

    virtual ~ITexture() = default;
    virtual Descriptor* GetDescriptor() = 0;
    virtual TextureType GetTextureType() const = 0;
};

}

