
#pragma once
#include "Base/BaseTypes.h"
#include "Base/Expected.h"
#include "Containers/Vector.h"
#include "Input/GamepadState.h"
#include "Input/KeyboardState.h"
#include "Input/MouseState.h"
#include "String/String.h"
#ifdef PDL_FEATURE_IMGUI
#include "InputManager_ImGui.h"
#endif
// Created on 2026-01-13 by sisco

namespace pdl
{

class InputManager
{
public:
    InputManager();
    Expected<void, String> Update();
    
    const KeyboardState& GetKeyboardState() const { return m_keyboardState; }
    const MouseState& GetMouseState() const { return m_mouseState; }
    const GamepadState& GetGamepadState(uint32 gamepadIndex) const { return m_gamepadStates[ gamepadIndex]; }
    uint32 GetGamepadCount() const { return m_gamepadStates.size(); }
private:
    Expected<void, String> UpdateInternal();
    
    KeyboardState m_keyboardState;
    MouseState m_mouseState;
    Vector<GamepadState> m_gamepadStates;
    
#ifdef PDL_FEATURE_IMGUI
    friend class InputManager_ImGui;
    InputManager_ImGui m_ImGui;
#endif
};

}

