#include <winerror.h>
#include <Input/InputStateUpdater.h>
#include "Input/GamepadState.h"
#include "Input/KeyboardState.h"
#include "String/String.h"

#define WINDOWS_LEAN_AND_MEAN
#include <Windows.h>
#include <xinput.h>

#include "Input/MouseState.h"

// Created on 2026-01-13 by sisco

namespace pdl
{
    Expected<uint32, String> InputStateUpdater::GetGamepadCount()
    {
        return XUSER_MAX_COUNT;
    }

    Expected<void, String> InputStateUpdater::UpdateGamepadState(uint32 index, GamepadState& outGamepadState)
    {
        XINPUT_STATE state = {};
        auto result = XInputGetState(index, &state);
        if (result == ERROR_SUCCESS)
        {

            outGamepadState.mConnected = true;

            auto& xgamepad = state.Gamepad;

            outGamepadState.mUp = xgamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP;
            outGamepadState.mDown = xgamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN;
            outGamepadState.mLeft = xgamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT;
            outGamepadState.mRight = xgamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT;

            outGamepadState.mA = xgamepad.wButtons & XINPUT_GAMEPAD_A;
            outGamepadState.mB = xgamepad.wButtons & XINPUT_GAMEPAD_B;
            outGamepadState.mX = xgamepad.wButtons & XINPUT_GAMEPAD_X;
            outGamepadState.mY = xgamepad.wButtons & XINPUT_GAMEPAD_Y;

            outGamepadState.mL = xgamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER;
            outGamepadState.mThumbLeft = xgamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB;
            outGamepadState.mR = xgamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER;
            outGamepadState.mThumbRight = xgamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB;

            outGamepadState.mSelect = xgamepad.wButtons & XINPUT_GAMEPAD_BACK;
            outGamepadState.mStart = xgamepad.wButtons & XINPUT_GAMEPAD_START;

            outGamepadState.mTriggerLeft = xgamepad.bLeftTrigger / 255.0f;
            outGamepadState.mTriggerRight = xgamepad.bRightTrigger / 255.0f;

            outGamepadState.mAnalogLeft = { xgamepad.sThumbLX / 32767.0f, xgamepad.sThumbLY / 32767.0f };
            outGamepadState.mAnalogRight = { xgamepad.sThumbRX / 32767.0f, xgamepad.sThumbRY / 32767.0f };
            
            return {};
        }
        else
        {
            outGamepadState = {};
            switch (result)
            {
                case ERROR_DEVICE_NOT_CONNECTED: outGamepadState.mConnected = false; return {};
                default: return Unexpected(String("Error getting gamepad state"));
            }
        }
    }

    Expected<void, String> InputStateUpdater::UpdateKeyboardState(KeyboardState& outKeyboardState)
    {
        for (auto i = 0; i < 255; ++i)
        {
            outKeyboardState.mKeys[i] = GetKeyState(i) & 0x8000;
        }
        return {};
    }

    Expected<void, String> InputStateUpdater::UpdateMouseState(MouseState& outMouseState)
    {
        POINT mousepos;
        auto result = GetCursorPos(&mousepos);
        if (result == 0) return Unexpected(String("Error getting mouse position"));
        
        outMouseState.mCursorPosition.x = (float)mousepos.x;
        outMouseState.mCursorPosition.y = (float)mousepos.y;
        outMouseState.mLMB = GetAsyncKeyState(VK_LBUTTON) & 0x8000;
        outMouseState.mRMB = GetAsyncKeyState(VK_RBUTTON) & 0x8000;
        outMouseState.mMMB = GetAsyncKeyState(VK_MBUTTON) & 0x8000;
        outMouseState.mX1MB = GetAsyncKeyState(VK_XBUTTON1) & 0x8000;
        outMouseState.mX2MB = GetAsyncKeyState(VK_XBUTTON2) & 0x8000;
        
        return {};
    }
}

