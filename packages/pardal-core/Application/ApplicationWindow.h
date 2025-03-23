
#pragma once
#include <Application/IApplicationWindow.h>
#include <Memory/UniquePointer.h>

// Created on 2025-03-22 by sisco

namespace pdl
{

class ApplicationWindow : public IApplicationWindow
{
public:
    ApplicationWindow(InitInfoBase initInfo);
    ~ApplicationWindow() override;
    
    void Update() override;

    void RequestClose() override { m_closeRequested = true; }
    void SetWindowSize(const Math::Vector2i& size) override;
    void SetWindowTitle(StringView title) override;
    void SetFullScreen(bool fullScreen) override;
    
    bool IsCloseRequested() const override { return m_closeRequested; }
    
    Math::Vector2i GetWindowSize() const override;
    String GetWindowTitle() const override;
    bool IsFullScreen() const override;
    void* GetNativeWindow() override;
    void* GetNativeModuleHandle() override;
    
    void AddResizeCallback(WindowResizeCallback _onResize) override { m_resizeCallbacks += _onResize; }
    void AddCloseCallback(WindowCloseCallback _onClose) override { m_closeCallbacks += _onClose; }
    void AddFocusCallback(WindowFocusCallback _onFocus) override { m_focusCallbacks += _onFocus; }
    void AddLostFocusCallback(WindowLostFocusCallback _onLostFocus) override { m_lostFocusCallbacks += _onLostFocus; }
    
    void AddMouseMoveCallback(MouseMoveCallback _onMouseMove) override { m_mouseMoveCallbacks += _onMouseMove; }
    void AddMouseWheelVCallback(MouseWheelCallback _onMouseWheelV) override { m_mouseWheelVCallbacks += _onMouseWheelV; }
    void AddMouseWheelHCallback(MouseWheelCallback _onMouseWheelH) override { m_mouseWheelHCallbacks += _onMouseWheelH; }
    
    void AddKeyUpCallback(KeyCallback _onKeyUp) override { m_keyUpCallbacks  += _onKeyUp; }
    void AddKeyDownCallback(KeyCallback _onKeyDown) override { m_keyDownCallbacks += _onKeyDown; }
    void AddKeyInputCallback(KeyCallback _onKeyInput) override { m_keyInputCallbacks +=_onKeyInput; }

private:
    friend class Impl;
    UniquePointer<Impl> m_windowImpl;

    EventCallbacks<WindowResizeCallback> m_resizeCallbacks;
    EventCallbacks<WindowCloseCallback> m_closeCallbacks;
    EventCallbacks<WindowFocusCallback> m_focusCallbacks;
    EventCallbacks<WindowLostFocusCallback> m_lostFocusCallbacks;
    EventCallbacks<MouseMoveCallback> m_mouseMoveCallbacks;
    EventCallbacks<MouseWheelCallback> m_mouseWheelVCallbacks;
    EventCallbacks<MouseWheelCallback> m_mouseWheelHCallbacks;
    EventCallbacks<KeyCallback> m_keyUpCallbacks;
    EventCallbacks<KeyCallback> m_keyDownCallbacks;
    EventCallbacks<KeyCallback> m_keyInputCallbacks;

    InitInfoBase m_initInfo;

    bool m_closeRequested = false;
    bool m_cursorVisible = true;
};

}

