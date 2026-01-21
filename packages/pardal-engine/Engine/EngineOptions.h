
#pragma once
#include "Base/Optional.h"
#include "Containers/UnorderedMap.h"
#include "String/String.h"
#include "String/StringCast.h"
#include "String/StringUtils.h"

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
    
    template<typename T = StringView>
    Optional<T> GetOption(StringView key) const;
    
    template <typename T>
    void SetOption(StringView key, T value); 

private:

    void ParseProgramArguments();
    void ParseIniFile();
    
    UnorderedMap<String, String> m_options;;
};

template <typename T>
Optional<T> EngineOptions::GetOption(StringView key) const
{
    if (m_options.contains(String(key)))
    {
        auto& val = m_options.at(String(key));
        return StringCast::FromString<T>(val);
    }
    return {};
}

template <typename T>
void EngineOptions::SetOption(StringView key, T value)
{
    m_options[String(key)] = StringCast::ToString(value);
}
}

