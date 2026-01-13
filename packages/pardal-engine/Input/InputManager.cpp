
#include <Input/InputManager.h>

#include "Input/InputStateUpdater.h"
#include "Log/Log.h"

// Created on 2026-01-13 by sisco

namespace pdl
{
    InputManager::InputManager() : m_ImGui(*this)
    {
        auto results = UpdateInternal();
        if (!results)
            pdlLogError("%s", results.error().c_str());
    }

    Expected<void, String> InputManager::Update()
    {
        auto updateResults = UpdateInternal();
#ifdef PDL_FEATURE_IMGUI
        m_ImGui.Update();
#endif
        
        return updateResults;
    }

    Expected<void, String> InputManager::UpdateInternal()
    {
        auto keyboardUpdateResults = InputStateUpdater::UpdateKeyboardState( m_keyboardState);
        if (!keyboardUpdateResults)
            return Unexpected(keyboardUpdateResults.error());
        
        auto mouseUpdateResults = InputStateUpdater::UpdateMouseState( m_mouseState);
        if (!mouseUpdateResults)
            return Unexpected(mouseUpdateResults.error());
        
        auto gamepadCountResults = InputStateUpdater::GetGamepadCount();
        if (!gamepadCountResults)
            return Unexpected(gamepadCountResults.error());
        
        if (gamepadCountResults.value() != m_gamepadStates.size())
        {
            m_gamepadStates.resize( gamepadCountResults.value());
            // Fire gamepad count changed event
        }
        
        for (uint32 i = 0; i < m_gamepadStates.size(); ++i)
        {
            auto gamepadUpdateResults = InputStateUpdater::UpdateGamepadState( i, m_gamepadStates[i]);
            if (!gamepadUpdateResults)
                return Unexpected(gamepadUpdateResults.error());       
        } 
        
        return {};
    }
}

