#ifdef PDL_FEATURE_IMGUI
#include "Input/InputManager_ImGui.h"

#include <imgui.h>

#include "Base/ServiceLocator.h"
#include "ImGui/ImGuiRenderer.h"
#include "Input/InputManager.h"


// Created on 2026-01-13 by sisco

namespace pdl
{
    InputManager_ImGui::InputManager_ImGui(InputManager& inputManager) :
        m_inputManager(inputManager)
    {
        //ServiceLocator<ImGuiRenderer>::Ref().RegisterRenderable(this);
    }

    InputManager_ImGui::~InputManager_ImGui()
    {
        //ServiceLocator<ImGuiRenderer>::Ref().UnregisterRenderable(this);
    }

    void InputManager_ImGui::ImGuiMenuSetup()
    {
        ImGuiRenderable::ImGuiMenuSetup();
        if (ImGui::BeginMenu("Engine"))
        {
            if (ImGui::MenuItem("Input manager", nullptr, m_enabled)) { m_enabled = !m_enabled;}
            ImGui::EndMenu();
        }
    }

    void InputManager_ImGui::ImGuiRender()
    {
        if (!m_enabled) return;
        
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

