
#pragma once
#include "Base/Expected.h"
#include "Memory/SharedPointer.h"
#include "String/String.h"

// Created on 2026-01-11 by Sisco

namespace pdl
{
    class InputManager;
    class Renderer;
    class IApplicationWindow;
    class EngineOptions;
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
    Expected<void, StringView> Shutdown();

    SharedPointer<Renderer> m_renderer;
    SharedPointer<IApplicationWindow> m_applicationWindow;
    SharedPointer<InputManager> m_inputManager;

    std::atomic<bool> m_closeRequested;
};

}

