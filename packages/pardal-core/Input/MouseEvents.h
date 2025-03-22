
#pragma once
#include <Base/Function.h>
#include <Math/Vector2.h>

// Created on 2025-03-22 by sisco

namespace pdl
{
    typedef Function<void(Math::Vector2, bool, bool, bool, unsigned int)> MouseMoveCallback;
    typedef Function<void(float)> MouseWheelCallback;

}

