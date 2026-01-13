
#pragma once

// Created on 2026-01-13 by sisco
#ifdef PDL_FEATURE_IMGUI

namespace pdl
{
class InputManager;

class InputManager_ImGui
{
public:
    InputManager_ImGui(InputManager& inputManager);
    void Update();
private:
    InputManager& m_inputManager;
};
}
#endif

