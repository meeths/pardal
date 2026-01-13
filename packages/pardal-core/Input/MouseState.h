
#pragma once
#include "ButtonState.h"
#include "Math/Vector2.h"

// Created on 2026-01-13 by sisco

namespace pdl
{

struct MouseState
{
    ButtonState mLMB;
    ButtonState mMMB;
    ButtonState mRMB;

    ButtonState mX1MB;
    ButtonState mX2MB;

    Math::Vector2 mCursorPosition = Math::Vector2(0, 0);
};

}

