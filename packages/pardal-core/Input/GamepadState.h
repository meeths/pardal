
#pragma once
#include "ButtonState.h"
#include "Math/Vector2.h"

// Created on 2026-01-13 by sisco

namespace pdl
{

struct GamepadState
{
    bool mConnected = false;

    ButtonState mUp;
    ButtonState mDown;
    ButtonState mLeft;
    ButtonState mRight;

    Math::Vector2 mAnalogRight = Math::Vector2(0, 0);
    Math::Vector2 mAnalogLeft = Math::Vector2(0, 0);

    float mTriggerLeft = 0;
    float mTriggerRight = 0;

    ButtonState mA;
    ButtonState mB;
    ButtonState mY;
    ButtonState mX;

    ButtonState mL;
    ButtonState mR;
    ButtonState mSelect;
    ButtonState mStart;

    ButtonState mThumbLeft;
    ButtonState mThumbRight;
};

}

