
#include <iostream>
#include "Engine/CoreSystems.h"

#include "Base/Expected.h"
#include "Engine/EngineOptions.h"
#include "Base/ServiceLocator.h"
#include "Log/Log.h"
#include "Memory/Memory.h"

#ifdef _WIN32
    #define WIN32_LEAN_AND_MEAN 1
    #include <windows.h>
#endif
// Created on 2026-03-01 by Sisco
namespace details
{
    static bool g_isInitialized = false;
}

namespace pdl
{
    Expected<void, StringView> CoreSystems::Initialize()
    {
        if (IsInitialized())
        {  
            return Unexpected<StringView>("CoreSystems already initialized");
        }
        
        Memory::Initialize();
        
        ServiceLocator<EngineOptions>::Create(EngineOptions::EInitFrom::All);
        ServiceLocator<Log>::Create();
        
        details::g_isInitialized = true;
        
        if (ServiceLocator<EngineOptions>::Ref().GetOption<bool>("wait_for_debugger").value_or(false))
        {
            std::cout << "Waiting for debugger...";
            while (!IsDebuggerPresent())
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
                std::cout << ".";
            }
        }

        return {};
    }

    bool CoreSystems::IsInitialized()
    {
        return details::g_isInitialized;
    }
}

