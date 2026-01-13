
#pragma once
#include "Base/BaseTypes.h"
#include "Base/Expected.h"

// Created on 2026-01-13 by sisco

namespace pdl
{
struct MouseState;
struct GamepadState;
struct KeyboardState;
class String;

class InputStateUpdater
{
public:
   static Expected<uint32, String> GetGamepadCount();
   static Expected<void, String> UpdateGamepadState(uint32 index, GamepadState& outGamepadState);
   static Expected<void, String> UpdateKeyboardState(KeyboardState& outKeyboardState);
   static Expected<void, String> UpdateMouseState(MouseState& outMouseState);
};
}

