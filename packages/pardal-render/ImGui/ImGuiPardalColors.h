
#pragma once
#include <Base/BaseDefines.h>

// Created on 2026-01-25 by sisco

namespace pdl
{

class ImGuiPardalColors
{
public:
    // Text colors
    DefineGlobalConstexprVariableAccessor(ImVec4, DisabledText, ImVec4(0.5f, 0.5f, 0.5f, 1.0f))
    DefineGlobalConstexprVariableAccessor(ImVec4, NormalText, ImVec4(1.0f, 1.0f, 1.0f, 1.0f))
    DefineGlobalConstexprVariableAccessor(ImVec4, TitleText, ImVec4(1.0f, 128.0f / 255.0f, 0.0f / 255.0f, 1.0f))
    DefineGlobalConstexprVariableAccessor(ImVec4, BlueText, ImVec4(53.0f / 255.0f, 79.0f / 255.0f, 185.0f / 255.0f, 1.0f))
    DefineGlobalConstexprVariableAccessor(ImVec4, GreenText, ImVec4(87.0f / 255.0f, 240.0f / 255.0f, 74.0f / 255.0f, 1.0f))
    DefineGlobalConstexprVariableAccessor(ImVec4, RedText, ImVec4(1.0f, 64.0f / 255.0f, 64.0f / 255.0f, 1.0f))
    
};

}

