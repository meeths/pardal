
#pragma once

// Created on 2026-01-13 by sisco

namespace pdl
{

struct ButtonState
{
    bool pressed = false;
    bool justPressed = false;
    bool justReleased = false;

    ButtonState& operator =(bool value)
    {
        justReleased = (pressed && !value);
        justPressed = (!pressed && value);
        pressed = value;
        return *this;
    }
};

}

