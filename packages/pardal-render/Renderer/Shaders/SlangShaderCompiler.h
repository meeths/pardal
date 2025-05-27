
#pragma once
#include <Renderer/Shaders/IShaderCompiler.h>
#include <slang.h>
#include <slang-com-ptr.h>
#include <Base/BaseTypes.h>
#include <Base/BaseDefines.h>

// Created on 2025-05-25 by sisco

namespace pdl
{

class SlangShaderCompiler : public IShaderCompiler
{
public:
	enum class Target : uint8
	{
		SPIRV
	};

	enum class CompilerOptions : uint32
	{
		EnableGLSLSupport	= 1 << 0,		
		TargetVulkan		= 1 << 1
	};

	struct InitInfo
	{
		Target m_target;
		CompilerOptions m_compilerOptions {};
	};
	SlangShaderCompiler(const InitInfo& initInfo);
	~SlangShaderCompiler() override = default;

	Expected<Vector<uint8>, String> CompileShader(CompileShaderInfo shaderInfo) override;

private:
	Slang::ComPtr<slang::IGlobalSession> m_globalSession;
	Slang::ComPtr<slang::ISession> m_session;
	Target m_target;
	CompilerOptions m_compilerOptions;
};

	DefineEnumMaskOperators(SlangShaderCompiler::CompilerOptions);
}


