#include <Renderer/Shaders/SlangShaderCompiler.h>
#include <Containers/Array.h>
#include <Log/Log.h>
#include <Base/DebugHelpers.h>
// Created on 2025-05-25 by sisco

namespace pdl
{
	SlangShaderCompiler::SlangShaderCompiler(const InitInfo& initInfo) : m_target(initInfo.m_target), m_compilerOptions(initInfo.m_compilerOptions)
	{
		SlangGlobalSessionDesc globalSessionDesc = {};

		globalSessionDesc.enableGLSL = (m_compilerOptions & CompilerOptions::EnableGLSLSupport) == CompilerOptions::EnableGLSLSupport;
		
		auto globalSessionResult = slang::createGlobalSession(&globalSessionDesc, m_globalSession.writeRef());
		if (SLANG_FAILED(globalSessionResult))
		{
			pdlLogError(slang::getLastInternalErrorMessage());
			return;
		}
		
		
		
		slang::SessionDesc sessionDesc = {};
		slang::TargetDesc targetDesc = {};
		switch (m_target)
		{
		case Target::SPIRV:
			targetDesc.format = SLANG_SPIRV;
			targetDesc.profile = m_globalSession->findProfile("spirv_1_5");
			break;
		default:
			pdlAssert(0 && "Unsupported Slang shader compiler target");
		}

		sessionDesc.targets = &targetDesc;
		sessionDesc.targetCount = 1;

		Vector<slang::CompilerOptionEntry> options;
		if (m_target == Target::SPIRV)
		{
			options.push_back({
					slang::CompilerOptionName::EmitSpirvDirectly,
					{slang::CompilerOptionValueKind::Int, 1, 0, nullptr, nullptr}
				}
			);
		}

		// No VulkanUseEntryPointName: Slang renames all entry points to "main" in
		// SPIRV output, which matches the LVK RenderPipelineDesc entryPoint* defaults.

		sessionDesc.compilerOptionEntries = options.data();
		sessionDesc.compilerOptionEntryCount = options.size();


		auto sessionResult = m_globalSession->createSession(sessionDesc, m_session.writeRef());
		if (SLANG_FAILED(sessionResult))
		{
			pdlLogError(slang::getLastInternalErrorMessage());
			return;
		}
	}

	Expected<Vector<uint8>, String> SlangShaderCompiler::CompileShader(CompileShaderInfo shaderInfo)
	{
		Slang::ComPtr<slang::IModule> slangModule;
		{
			Slang::ComPtr<slang::IBlob> diagnosticsBlob;
			slangModule = m_session->loadModuleFromSourceString(
				shaderInfo.m_entryPoint.data(),
				shaderInfo.m_shaderSourceFile.data(),
				shaderInfo.m_shaderSource.data(),
				diagnosticsBlob.writeRef());

			if (!slangModule)
			{
				if (diagnosticsBlob)
				{
					return Unexpected<String>(static_cast<const char*>(diagnosticsBlob->getBufferPointer()));
				}
				return Unexpected<String>("Error loading module");
			}
		}

		Slang::ComPtr<slang::IEntryPoint> entryPoint;
		{
			Slang::ComPtr<slang::IBlob> diagnosticsBlob;
			slangModule->findEntryPointByName(shaderInfo.m_entryPoint.data(), entryPoint.writeRef());
			if (!entryPoint)
			{
				return Unexpected<String>("Error getting entry point");
			}
		}

		Array<slang::IComponentType*, 2> componentTypes =
		{
			slangModule,
			entryPoint
		};

		Slang::ComPtr<slang::IComponentType> composedProgram;
		{
			Slang::ComPtr<slang::IBlob> diagnosticsBlob;
			SlangResult result = m_session->createCompositeComponentType(
				componentTypes.data(),
				componentTypes.size(),
				composedProgram.writeRef(),
				diagnosticsBlob.writeRef());
			if (SLANG_FAILED(result))
			{
				if (diagnosticsBlob)
				{
					return Unexpected<String>(static_cast<const char*>(diagnosticsBlob->getBufferPointer()));
				}
				return Unexpected<String>("Error creating composite program");
			}
		}

		Slang::ComPtr<slang::IComponentType> linkedProgram;
		{
			Slang::ComPtr<slang::IBlob> diagnosticsBlob;
			SlangResult result = composedProgram->link(
				linkedProgram.writeRef(),
				diagnosticsBlob.writeRef());
			if (SLANG_FAILED(result))
			{
				if (diagnosticsBlob)
				{
					return Unexpected<String>(static_cast<const char*>(diagnosticsBlob->getBufferPointer()));
				}
				return Unexpected<String>("Error linking module");
			}
		}

		Slang::ComPtr<slang::IBlob> targetCode;
		{
			Slang::ComPtr<slang::IBlob> diagnosticsBlob;
			SlangResult result = linkedProgram->getEntryPointCode(
				0,
				0,
				targetCode.writeRef(),
				diagnosticsBlob.writeRef());
			if (SLANG_FAILED(result))
			{
				if (diagnosticsBlob)
				{
					return Unexpected<String>(static_cast<const char*>(diagnosticsBlob->getBufferPointer()));
				}
				return Unexpected<String>("Error getting target code");
			}
		}

		return Vector<uint8>(
			static_cast<const uint8*>(targetCode->getBufferPointer()),
			static_cast<const uint8*>(targetCode->getBufferPointer()) + targetCode->getBufferSize());
	}
}
