
#pragma once
#include <Base/BaseDefines.h>
#include <Math/Vector2i.h>
#include <String/String.h>
#include <Input/MouseEvents.h>
#include <Base/EventCallbacks.h>
#include <Input/KeyEvents.h>

// Created on 2025-03-22 by sisco

namespace pdl
{
typedef Callback<void(Math::Vector2i)> WindowResizeCallback;
typedef Callback<void()> WindowCloseCallback;
typedef Callback<void()> WindowFocusCallback;
typedef Callback<void()> WindowLostFocusCallback;

class IApplicationWindow
{
public:
    struct InitInfoBase
    {
        bool m_fullScreen = false;
        Math::Vector2i m_windowSize = Math::Vector2i(1280, 720);
        String m_windowTitle = "Pardal";

        bool m_showCursor = true;
    };

    IApplicationWindow() = default;
    virtual ~IApplicationWindow() = default;
    DeclareNonCopyable(IApplicationWindow);

    virtual void Update() = 0;
    
    virtual void RequestClose() = 0;
    virtual void SetWindowSize(const Math::Vector2i& size) = 0;
    virtual void SetWindowTitle(StringView title) = 0;
    virtual void SetFullScreen(bool fullScreen) = 0;
    
    virtual bool IsCloseRequested() const = 0;
    virtual Math::Vector2i GetWindowSize() const = 0;
    virtual String GetWindowTitle() const = 0;
    virtual bool IsFullScreen() const = 0;

    virtual void* GetNativeWindow() = 0;
    virtual void* GetNativeModuleHandle() = 0;

    // Events
    virtual void AddResizeCallback(WindowResizeCallback _onResize) = 0;
    virtual void AddCloseCallback(WindowCloseCallback _onClose) = 0;
    virtual void AddFocusCallback(WindowFocusCallback _onFocus) = 0;
    virtual void AddLostFocusCallback(WindowLostFocusCallback _onLostFocus) = 0;

    virtual void AddMouseMoveCallback(MouseMoveCallback _onMouseMove) = 0;
    virtual void AddMouseWheelVCallback(MouseWheelCallback _onMouseWheelV) = 0;
    virtual void AddMouseWheelHCallback(MouseWheelCallback _onMouseWheelH) = 0;

    virtual void AddKeyUpCallback(KeyCallback _onKeyUp) = 0;
    virtual void AddKeyDownCallback(KeyCallback _onKeyDown) = 0;
    virtual void AddKeyInputCallback(KeyCallback _onKeyInput) = 0;
};

}

