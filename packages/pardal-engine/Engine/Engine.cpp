
#include <Engine/Engine.h>

#include "CoreSystems.h"
#include "EngineOptions.h"
#include "Application/ApplicationWindow.h"
#include "Application/IApplicationWindow.h"
#include "ImGui/ImGuiPerfWidget.h"
#include "Input/InputManager.h"
#include "Log/Log.h"
#include "Log/LoggerStdout.h"
#include "Memory/Memory.h"
#include "Render/RenderGraph.h"
#include "Render/Passes/DebugRenderGraphPass.h"
#include "Render/Passes/EditorRenderGraphPass.h"
#include "Renderer/IRenderer.h"
#include "Renderer/IRHIContext.h"
#include "Renderer/RenderererDevices.h"
#include "Time/Chronometer.h"

#ifdef PDL_VULKAN
#include "Renderer/Vulkan/VulkanRenderer.h"
#endif

// Created on 2026-01-11 by Sisco
namespace Defaults
{
	pdlMaybeUnused constexpr const char* RendererBackend = "Vulkan";
	pdlMaybeUnused constexpr const char* ApplicationName = "Pardal";
	pdlMaybeUnused constexpr const char* WindowTitle = "Pardal engine";
	pdlMaybeUnused constexpr bool Fullscreen = false;
	pdlMaybeUnused constexpr bool EnableValidation = false;
	pdlMaybeUnused constexpr bool VSync = false;
	pdlMaybeUnused constexpr bool HDR = false;
}

namespace pdl
{
	Engine::Engine(const EngineOptions& options)
		: m_engineOptions(options)
	{
		auto initializeResults = Initialize();
		if (!initializeResults)
		{
			pdlLogError("Engine initialization failed: %s", initializeResults.error().data());
		}
	}

	void Engine::Run()
	{
		Chronometer frameTimer;
		frameTimer.Start();
		do
		{
			const float deltaTime = frameTimer.Lap<float, pdl::TimeTypes::Seconds>();
			m_perfWidget->Update(deltaTime, deltaTime);
			auto inputManagerUpdateResults = m_inputManager->Update();
			if (!inputManagerUpdateResults)
			{
				pdlLogError("InputManager update failed: %s", inputManagerUpdateResults.error().data());
				pdlHalt();
			}
			m_applicationWindow->Update();

			m_renderGraph->Update(deltaTime, *m_inputManager);
			m_renderGraph->Execute(*m_renderer->GetRHIContext());

			pdlLogFlush();

		} while (!IsCloseRequested());

	}

	Engine::~Engine()
	{
		auto shutdownResults = Shutdown();
		if (!shutdownResults)
		{
			pdlLogError("Engine shutdown failed: %s", shutdownResults.error().data());
		}
		
	}

	Expected<void, StringView> Engine::Initialize()
	{
		pdlAssert(CoreSystems::IsInitialized());
		
#ifndef PDL_RELEASE
		ServiceLocator<Log>::Ref().RegisterLogger(pdl::MakeSharedPointer<LoggerStdout>());
#endif
		
		IApplicationWindow::InitInfoBase windowInitInfo;
		 
		windowInitInfo.m_windowTitle = m_engineOptions.GetOption("window_title").value_or(Defaults::WindowTitle);
		windowInitInfo.m_fullScreen = m_engineOptions.GetOption<bool>("fullscreen").value_or(Defaults::Fullscreen);
		
		if (auto windowHandleOption = m_engineOptions.GetOption<unsigned long long>("parent_window"))
		{
			windowInitInfo.m_parentWindow = reinterpret_cast<void*>(windowHandleOption.value());
		}
		
		if (const auto windowRectOption =  m_engineOptions.GetOption<Math::Vector4>("window_rect"))
		{
			auto windowRect = windowRectOption.value();
			windowInitInfo.m_windowPosition = {windowRect.x, windowRect.y};
			windowInitInfo.m_windowSize = {windowRect.z, windowRect.w};
		}
		
		m_applicationWindow = MakeSharedPointer<ApplicationWindow>(windowInitInfo);
		m_applicationWindow->AddCloseCallback([this](){ m_closeRequested = true; });

		auto initializeRendererResults = InitializeRenderer();
		if (!initializeRendererResults)
		{
			return Unexpected(initializeRendererResults.error());
		}

		m_inputManager = MakeSharedPointer<InputManager>();

		auto initializeRenderGraphResults = InitializeRenderGraph();
		if (!initializeRenderGraphResults)
		{
			return Unexpected(initializeRenderGraphResults.error());
		}

		m_perfWidget = MakeUniquePointer<ImGuiPerfWidget>();
		
		return {};
	}

	Expected<void, StringView> Engine::InitializeRenderer()
	{
		const auto backendStr = m_engineOptions.GetOption("renderer_backend").value_or(Defaults::RendererBackend);
		const auto deviceType = StringCast::FromString<RenderDeviceType>(backendStr);

		if (deviceType == RenderDeviceType::None)
		{
			return Unexpected<StringView>("Unknown renderer backend");
		}

		IRenderer::InitInfo initInfo
		{
			.m_applicationName   = m_engineOptions.GetOption("application_name").value_or(Defaults::ApplicationName),
			.m_applicationWindow = static_cast<ApplicationWindow&>(*m_applicationWindow),
			.m_enableValidation  = m_engineOptions.GetOption<bool>("enable_validation").value_or(Defaults::EnableValidation),
			.m_useVSync          = m_engineOptions.GetOption<bool>("vsync").value_or(Defaults::VSync),
			.m_useHDR            = m_engineOptions.GetOption<bool>("hdr").value_or(Defaults::HDR),
			.m_preferredDeviceIndex = m_engineOptions.GetOption<int>("device_index").value_or(-1),
		};

		m_renderer = CreateRenderer(deviceType, initInfo);
		if (!m_renderer)
		{
			return Unexpected<StringView>("Failed to create renderer");
		}

		return {};
	}

	Expected<void, StringView> Engine::InitializeRenderGraph()
	{
		m_renderGraph = MakeUniquePointer<RenderGraph>();

#ifdef PDL_FEATURE_IMGUI
		auto editorPass = MakeUniquePointer<EditorRenderGraphPass>();
		editorPass->SetWindow(*m_applicationWindow);
		m_renderGraph->AddPass(std::move(editorPass));
		m_renderGraph->AddPass(MakeUniquePointer<DebugRenderGraphPass>());
#endif

		m_renderGraph->Build(*m_renderer->GetRHIContext());

		// Resize render targets whenever the window changes resolution.
		// The renderer's own resize callback (registered earlier) already calls
		// WaitIdle and recreates the swapchain before ours fires.
		m_applicationWindow->AddResizeCallback([this](Math::Vector2 newSize)
		{
			if (newSize.x <= 0 || newSize.y <= 0)
				return;
			m_renderGraph->Resize(
				*m_renderer->GetRHIContext(),
				static_cast<uint32>(newSize.x),
				static_cast<uint32>(newSize.y));
		});

		return {};
	}

	Expected<void, StringView> Engine::Shutdown()
	{
		m_renderer->GetRHIContext()->WaitIdle();
		m_renderGraph.reset();
		m_renderer.reset();
		m_applicationWindow.reset();
		return {};
	}
}

