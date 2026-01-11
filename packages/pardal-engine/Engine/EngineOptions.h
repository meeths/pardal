
#pragma once
#include "Base/Optional.h"
#include "Containers/UnorderedMap.h"
#include "Containers/Vector.h"
#include "String/String.h"
#include <string>

// Created on 2026-01-11 by Sisco

namespace pdl
{

class EngineOptions
{
public:
    
    enum class EInitFrom
    {
        ProgramArguments = 1 << 0,
        IniFile = 1 << 1,
        All = ProgramArguments | IniFile
    };
    
    EngineOptions(EInitFrom initFrom = EInitFrom::All);
    Optional<String> GetOption(StringView key) const; 

private:

    void ParseProgramArguments();
    void ParseIniFile();
    
    UnorderedMap<String, String> m_options;;
};

}

