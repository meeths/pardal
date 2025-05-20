
#pragma once
#include "RendererTypes.h"
#include "Containers/Vector.h"

// Created on 2025-05-04 by sisco

namespace pdl
{
class ICommandBuffer;
class IRenderBuffer;
class ITextureView;

enum class TextureDescriptorHandle : uint32 { Invalid = 0xFFFFFFFF };
enum class BufferDescriptorHandle : uint32 { Invalid = 0xFFFFFFFF };
    
class BindlessDescriptors
{
public:
    struct BindlessDescriptorInfo
    {
        Vector<DescriptorTypes> m_descriptorTypes = { DescriptorTypes::UniformBuffer, DescriptorTypes::StorageBuffer, DescriptorTypes::CombinedSampler };
        uint32 m_descriptorCount = 64 * 1024;
    };

    BindlessDescriptors() = default;
    virtual ~BindlessDescriptors() = default;

    virtual TextureDescriptorHandle StoreTexture(ITextureView* textureView) = 0;
    virtual BufferDescriptorHandle StoreBuffer(IRenderBuffer* buffer) = 0;

    virtual void WriteDescriptors() = 0;
    
    virtual bool Initialize(BindlessDescriptorInfo& info) = 0;

    virtual bool Bind(ICommandBuffer* commandBuffer) = 0; 
};

}

