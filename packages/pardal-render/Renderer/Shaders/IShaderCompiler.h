
#pragma once
#include <Renderer/Shaders/CompileShaderInfo.h>
#include <Base/BaseTypes.h>
#include <Containers/Vector.h>
#include <Base/Expected.h>

// Created on 2025-05-25 by sisco

namespace pdl
{

class IShaderCompiler
{
public:
	virtual ~IShaderCompiler() = default;
	virtual Expected<Vector<uint8>, String> CompileShader(CompileShaderInfo shaderInfo) = 0;
};

}

