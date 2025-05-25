
#pragma once
#include "ICommandBuffer.h"
#include "Containers/Vector.h"

// Created on 2025-05-24 by sisco

namespace pdl
{
    class IShaderObject;
}

namespace pdl
{

class IShader
{
public:

    struct ShaderDescriptor
    {
        Vector<IShaderObject*> m_shaderObjects;
    };
    
    virtual ~IShader() = default;

    virtual void Bind(ICommandBuffer* commandBuffer) = 0;
};

}

