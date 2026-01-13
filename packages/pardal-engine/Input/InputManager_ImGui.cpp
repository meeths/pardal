#ifdef PDL_FEATURE_IMGUI
#include <Input/InputManager_ImGui.h>
#include <Input/InputManager.h>
#include <imgui.h>

// Created on 2026-01-13 by sisco

namespace pdl
{
    InputManager_ImGui::InputManager_ImGui(InputManager& inputManager) :
        m_inputManager(inputManager)
    {
        
    }

    void InputManager_ImGui::Update()
    {
        ImGui::Begin("Input");
        ImGui::Text("Mouse position: (%.1f, %.1f)", ImGui::GetIO().MousePos.x, ImGui::GetIO().MousePos.y);
        for (uint32 i = 0; i < m_inputManager.GetGamepadCount(); ++i)
        {
            const GamepadState& gamepadState = m_inputManager.GetGamepadState(i);
            ImGui::Text("Gamepad %u: %s", i, gamepadState.mConnected ? "connected" : "disconnected");
        }
        ImGui::End();
    }
}
#endif

