#pragma once
#include "Base/Expected.h"
#include "Memory/SharedPointer.h"
#include "Memory/UniquePointer.h"
#include "String/String.h"
#include "Threading/Atomic.h"

// Created on 2026-01-11 by Sisco

namespace pdl
{
    class ImGuiPerfWidget;
    class InputManager;
    class IApplicationWindow;
    class IRenderer;
    class EngineOptions;
    class RenderGraph;

    class Engine
    {
    public:
        Engine(const EngineOptions& options);
        void Run();
        ~Engine();

    private:
        bool IsCloseRequested() const { return m_closeRequested; } ;
        const EngineOptions& m_engineOptions;

        Expected<void, StringView> Initialize();
        Expected<void, StringView> InitializeRenderer();
        Expected<void, StringView> InitializeRenderGraph();
        Expected<void, StringView> Shutdown();

        SharedPointer<IApplicationWindow> m_applicationWindow;
        SharedPointer<InputManager> m_inputManager;
        UniquePointer<IRenderer> m_renderer;
        UniquePointer<RenderGraph> m_renderGraph;

        UniquePointer<ImGuiPerfWidget> m_perfWidget;

        Atomic<bool> m_closeRequested;
    };
}
