
#include <Engine/EngineOptions.h>
#include "Application/ProgramArguments.h"

#if defined(_WIN32)
#   include <windows.h>
#endif

// Created on 2026-01-11 by Sisco

namespace pdl
{
    EngineOptions::EngineOptions(EInitFrom initFrom)
    {
        // Init order is: IniFile, ProgramArguments

        if (static_cast<int>(initFrom) & static_cast<int>(EInitFrom::IniFile))
        {
            ParseIniFile();
        }
        if (static_cast<int>(initFrom) & static_cast<int>(EInitFrom::ProgramArguments))
        {
            ParseProgramArguments();
        }
 
    }

    void EngineOptions::ParseProgramArguments()
    {
        auto options = ProgramArguments::GetProgramArguments();
        m_options.insert(options.begin(), options.end());
    }

    void EngineOptions::ParseIniFile()
    {
        
    }
}

