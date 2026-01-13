
#pragma once
#include "ButtonState.h"
#include "Containers/Array.h"

// Created on 2026-01-13 by sisco

namespace pdl
{

struct KeyboardState
{
    Array<ButtonState, 512> mKeys = {};
};

}

