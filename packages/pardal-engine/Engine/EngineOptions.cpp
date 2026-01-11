
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

    Optional<String> EngineOptions::GetOption(StringView key) const
    {
        if (m_options.contains(key))
            return m_options.at(key);
        return {};
    }

    // moved inline into header: EngineOptions::ParseArgsLine

    void EngineOptions::ParseProgramArguments()
    {
        auto options = pdl::ProgramArguments::GetProgramArguments();
        m_options.concat(options);
    }

    void EngineOptions::ParseIniFile()
    {
        
    }
}

