
#pragma once
#include "RendererTypes.h"
#include "Containers/Vector.h"
#include "String/String.h"

// Created on 2025-05-24 by sisco

namespace pdl
{
class IDescriptorSet;
class IShaderObject
{
public:
    struct ShaderObjectDescriptor
    {
        ShaderType m_shaderType;
        const Vector<uint8>* m_shaderData;
        StringView m_entryPoint;
        IDescriptorSet* m_descriptorSet = nullptr;
        ShaderObjectDescriptor* m_nextShaderObject = nullptr;
    };
    
    virtual ~IShaderObject() = default;
    virtual ShaderType GetShaderType() const = 0;
};

}

