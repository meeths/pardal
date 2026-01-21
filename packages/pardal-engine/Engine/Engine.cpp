
#include <Engine/Engine.h>

#include "EngineOptions.h"
#include "Application/ApplicationWindow.h"
#include "Application/IApplicationWindow.h"
#include "Input/InputManager.h"
#include "Log/Log.h"
#include "Log/LoggerStdout.h"
#include "Memory/Memory.h"
#include "Renderer/IRenderDevice.h"
#include "Renderer/Renderer.h"
#include "Time/Chronometer.h"

// Created on 2026-01-11 by Sisco
namespace Defaults
{
	constexpr const char* RendererBackend = "Vulkan";
	constexpr const char* ApplicationName = "Pardal";
	constexpr const char* WindowTitle = "Pardal engine";
	constexpr bool Fullscreen = false;
	constexpr bool EnableValidation = false;
	constexpr bool VSync = false;
	constexpr bool HDR = false;
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
			auto deltaTSeconds = frameTimer.Lap<float, TimeTypes::Seconds>();

			auto inputManagerUpdateResults = m_inputManager->Update();
			if (!inputManagerUpdateResults)
			{
				pdlLogError("InputManager update failed: %s", inputManagerUpdateResults.error().data());
				pdlHalt();
			}

			m_renderer->Update(deltaTSeconds);
			m_renderer->BeginFrame();

			Math::Rectangle windowSize = {{0,0}, {m_applicationWindow->GetWindowSize().x, m_applicationWindow->GetWindowSize().y}};
			Math::Rectanglei scissorSize = {{0,0}, {m_applicationWindow->GetWindowSize().x, m_applicationWindow->GetWindowSize().y}};
			m_renderer->SetViewport(windowSize);
			m_renderer->SetScissor(scissorSize);

			m_renderer->BeginRenderPass(m_renderer->GetMainRenderPass());

			m_renderer->EndRenderPass();
			m_renderer->EndFrame();
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
		Memory::Initialize();
#ifndef PDL_RELEASE
		Log::Instance().RegisterLogger(pdl::MakeSharedPointer<LoggerStdout>());
#endif
		
		IApplicationWindow::InitInfoBase windowInitInfo;
		 
		windowInitInfo.m_windowTitle = m_engineOptions.GetOption("window_title").value_or(Defaults::WindowTitle);
		windowInitInfo.m_fullScreen = m_engineOptions.GetOption<bool>("fullscreen").value_or(Defaults::Fullscreen);
		
		m_applicationWindow = MakeSharedPointer<ApplicationWindow>(windowInitInfo);
		m_applicationWindow->AddCloseCallback([this](){ m_closeRequested = true; });
		
		m_inputManager = MakeSharedPointer<InputManager>();
		
		auto initializeRendererResults = InitializeRenderer();
		if (!initializeRendererResults)
			return initializeRendererResults;
		
		return {};
	}

	Expected<void, StringView> Engine::InitializeRenderer()
	{
		auto rendererOption = m_engineOptions.GetOption("renderer_backend").value_or(Defaults::RendererBackend);
		
		IRenderDevice::InitInfoBase rendererInitInfo
		{
			.m_deviceType = StringCast::FromString<RenderDeviceType>(rendererOption),
			.m_applicationName = m_engineOptions.GetOption("window_title").value_or(Defaults::ApplicationName),
			.m_applicationWindow = *static_cast<ApplicationWindow*>(m_applicationWindow.Get()),
			.m_enableValidation = m_engineOptions.GetOption<bool>("enable_validation").value_or(Defaults::EnableValidation),
			.m_useVSync = m_engineOptions.GetOption<bool>("vsync").value_or(Defaults::VSync),
			.m_useHDR = m_engineOptions.GetOption<bool>("hdr").value_or(Defaults::HDR)
		};
		
		m_renderer = MakeSharedPointer<Renderer>();
		bool initSuccessful = m_renderer->InitializeRenderDevice(rendererInitInfo); 
		
		return initSuccessful ? Expected<void, StringView>{} : Unexpected<StringView>("Failed to initialize renderer");
	}

	Expected<void, StringView> Engine::Shutdown()
	{
		m_applicationWindow.Reset();
		return {};
	}
}

