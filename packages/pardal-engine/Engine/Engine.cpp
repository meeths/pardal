
#include <Engine/Engine.h>

#include "CoreSystems.h"
#include "EngineOptions.h"
#include "Application/ApplicationWindow.h"
#include "Application/IApplicationWindow.h"
#include "Input/InputManager.h"
#include "Log/Log.h"
#include "Log/LoggerStdout.h"
#include "Memory/Memory.h"
#include "Renderer/IRenderer.h"
#include "Time/Chronometer.h"

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

			auto inputManagerUpdateResults = m_inputManager->Update();
			if (!inputManagerUpdateResults)
			{
				pdlLogError("InputManager update failed: %s", inputManagerUpdateResults.error().data());
				pdlHalt();
			}
			m_applicationWindow->Update();

			
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
		
		
		return {};
	}

	Expected<void, StringView> Engine::InitializeRenderer()
	{
		pdlMaybeUnused auto rendererOption = m_engineOptions.GetOption("renderer_backend").value_or(Defaults::RendererBackend);
		
		return {};
	
	}

	Expected<void, StringView> Engine::Shutdown()
	{
		m_applicationWindow.reset();
		return {};
	}
}

