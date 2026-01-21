
#pragma once

// Created on 2026-01-13 by sisco
#ifdef PDL_FEATURE_IMGUI
#include "ImGui/ImGuiRenderable.h"

namespace pdl
{
class InputManager;

class InputManager_ImGui : public ImGuiRenderable
{
public:
    InputManager_ImGui(InputManager& inputManager);
    ~InputManager_ImGui();
    void ImGuiRender() override;
private:
    InputManager& m_inputManager;
};
}
#endif

