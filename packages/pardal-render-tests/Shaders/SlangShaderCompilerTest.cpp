#include <gtest/gtest.h>

#include "Renderer/Shaders/SlangShaderCompiler.h"

namespace pdl { namespace Tests
{

struct SlangShaderCompilerTest : testing::Test
{
	SlangShaderCompilerTest()
	{
	}
	~SlangShaderCompilerTest()
	{
	}
	void SetUp() override
	{
		pdl::SlangShaderCompiler::InitInfo shaderCompilerInitInfo { .m_target = pdl::SlangShaderCompiler::Target::SPIRV };
		shaderCompiler = new SlangShaderCompiler(shaderCompilerInitInfo);
	}
	
	pdl::SlangShaderCompiler* shaderCompiler;
};
	
	TEST_F(SlangShaderCompilerTest, SimpleSPIRVCompilationSucceeds)
	{

		const char* shortestShader =
			"RWStructuredBuffer<float> result;"
			"[shader(\"compute\")]"
			"[numthreads(1,1,1)]"
			"void computeMain(uint3 threadId : SV_DispatchThreadID)"
			"{"
			"    result[threadId.x] = threadId.x;"
			"}";

		auto compileResults = shaderCompiler->CompileShader({"TestShader", shortestShader, "computeMain"});
		ASSERT_TRUE (compileResults.has_value());
	}

	TEST_F(SlangShaderCompilerTest, BrokenSPIRVCompilationFails)
	{
		const char* brokenShader =
			"RWStructuredBuffer<float> result;"
			"[shader(\"compute\")]"
			"[numthreads(1,1,1)]"
			"void computeMain(uint3 threadId : SV_DispatchThreadID)"
			"{"
			"    result[threadId.x] = threadId.x; and some shit"
			"}";

		auto compileResults = this-> shaderCompiler->CompileShader({"TestShader", brokenShader, "computeMain"});
		ASSERT_FALSE(compileResults.has_value());
		ASSERT_FALSE(compileResults.error().empty());

	}
}}
