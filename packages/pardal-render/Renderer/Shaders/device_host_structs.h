#pragma once
// Created on 2025-04-29 by sisco
#include "device_host_types.h"

struct PerFrameInfo
{
    mat4 projection;
    mat4 view;
    vec3 eyePos;
    int frameIndex;
    float deltaTime;
    float totalTime;
};

struct BasePushConstants
{
    mat4 transform;
    vec4 color;
};