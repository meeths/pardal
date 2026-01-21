
#include <Input/InputManager.h>

#include "Application/IApplicationWindow.h"
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
        return updateResults;
    }

    void InputManager::ConfigureApplicationWindow(IApplicationWindow& window)
    {
        window.AddMouseWheelHCallback([this](auto delta){ m_mouseState.mWheelH += delta; m_mouseWheelHCallbacks(delta);});
        window.AddMouseWheelVCallback([this](auto delta){ m_mouseState.mWheelH += delta; m_mouseWheelVCallbacks(delta);});
        window.AddKeyDownCallback([this](auto key) {m_keyDownCallbacks(key);});
        window.AddKeyUpCallback([this](auto key) {m_keyUpCallbacks(key);});
        window.AddKeyInputCallback([this](auto key) {m_keyInputCallbacks(key);});
        window.AddMouseMoveCallback([this](Math::Vector2 pos, bool lButton, bool rButton, bool mButton, unsigned int mods) 
        {
            m_mouseMoveCallbacks(pos, lButton, rButton, mButton, mods);
        });

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
            m_gamepadCountChanged(static_cast<uint8>(gamepadCountResults.value()));
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

