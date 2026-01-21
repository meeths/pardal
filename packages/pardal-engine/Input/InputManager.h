
#pragma once
#include "Base/BaseTypes.h"
#include "Base/EventCallbacks.h"
#include "Base/Expected.h"
#include "Containers/Vector.h"
#include "Input/GamepadEvents.h"
#include "Input/GamepadState.h"
#include "Input/KeyboardState.h"
#include "Input/KeyEvents.h"
#include "Input/MouseEvents.h"
#include "Input/MouseState.h"
#include "String/String.h"

#ifdef PDL_FEATURE_IMGUI
#include "InputManager_ImGui.h"
#endif
// Created on 2026-01-13 by sisco

namespace pdl
{
class IApplicationWindow;

class InputManager
{
public:
    InputManager();
    Expected<void, String> Update();
    
    const KeyboardState& GetKeyboardState() const { return m_keyboardState; }
    const MouseState& GetMouseState() const { return m_mouseState; }
    const GamepadState& GetGamepadState(uint32 gamepadIndex) const { return m_gamepadStates[ gamepadIndex]; }
    uint32 GetGamepadCount() const { return m_gamepadStates.size(); }
    
    void ConfigureApplicationWindow(IApplicationWindow& window);
    
    void AddMouseMoveCallback(MouseMoveCallback _onMouseMove) { m_mouseMoveCallbacks += _onMouseMove; };
    void AddMouseWheelVCallback(MouseWheelCallback _onMouseWheelV) { m_mouseWheelVCallbacks += _onMouseWheelV;};
    void AddMouseWheelHCallback(MouseWheelCallback _onMouseWheelH) { m_mouseWheelHCallbacks += _onMouseWheelH;};

    void AddKeyUpCallback(KeyCallback _onKeyUp) { m_keyUpCallbacks += _onKeyUp;};
    void AddKeyDownCallback(KeyCallback _onKeyDown) { m_keyDownCallbacks += _onKeyDown;};
    void AddKeyInputCallback(KeyCallback _onKeyInput) { m_keyInputCallbacks += _onKeyInput;};
    
    void AddGamepadCountChangedCallback(GamepadCountChangedCallback _onGamepadCountChanged) { m_gamepadCountChanged += _onGamepadCountChanged;}

private:
    
    Expected<void, String> UpdateInternal();
    
    KeyboardState m_keyboardState;
    MouseState m_mouseState;
    Vector<GamepadState> m_gamepadStates;

    EventCallbacks<MouseMoveCallback> m_mouseMoveCallbacks;
    EventCallbacks<MouseWheelCallback> m_mouseWheelVCallbacks;
    EventCallbacks<MouseWheelCallback> m_mouseWheelHCallbacks;
    EventCallbacks<KeyCallback> m_keyUpCallbacks;
    EventCallbacks<KeyCallback> m_keyDownCallbacks;
    EventCallbacks<KeyCallback> m_keyInputCallbacks;
    EventCallbacks<GamepadCountChangedCallback> m_gamepadCountChanged;

#ifdef PDL_FEATURE_IMGUI
    friend class InputManager_ImGui;
    InputManager_ImGui m_ImGui;
#endif
};

}

