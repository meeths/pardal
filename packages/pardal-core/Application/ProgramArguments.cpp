
#include <Application/ProgramArguments.h>
#include "String/String.h"

// Created on 2026-01-11 by Sisco

namespace pdl
{
#ifndef PDL_PLATFORM_WINDOWS
    String ProgramArguments::GetCommandLine()
    {
        // Non-Windows platforms not implemented yet
        return String();
    }
#endif

    void ProgramArguments::ParseArgsLine(StringView cmdLine, UnorderedMap<String, String>& outOptions)
    {
        // Tokenize respecting quotes
        Vector<String> tokens;
        tokens.reserve(8);
        bool inQuotes = false;
        String current;
        const char* data = cmdLine.data();
        size_t len = cmdLine.size();
        for (size_t i = 0; i < len; ++i)
        {
            char c = data[i];
            if (c == '"')
            {
                inQuotes = !inQuotes;
                current.push_back('"');
                continue;
            }
            if (!inQuotes && (c == ' ' || c == '\t'))
            {
                if (!current.empty())
                {
                    tokens.push_back(current);
                    current.clear();
                }
            }
            else
            {
                current.push_back(c);
            }
        }
        if (!current.empty())
            tokens.push_back(current);

        if (tokens.empty())
            return;

        auto trimQuotes = [](const String& s) -> String
        {
            if (s.size() >= 2 && s[0] == '"' && s[s.size() - 1] == '"')
            {
                return String(StringView(s.data() + 1, s.size() - 2));
            }
            return s;
        };

        auto countLeadingDashes = [](const String& s) -> size_t
        {
            size_t n = 0;
            while (n < s.size() && s[n] == '-') ++n;
            return n;
        };

        auto isValidKeyToken = [&](const String& s) -> bool
        {
            size_t d = countLeadingDashes(s);
            return (d == 1 || d == 2) && (s.size() > d);
        };

        // Skip first token (exe path)
        size_t i = 1;
        while (i < tokens.size())
        {
            const String& tok = tokens[i];
            size_t dashCount = countLeadingDashes(tok);

            // Only accept tokens that start with exactly 1 or 2 dashes and have a non-empty name
            if (dashCount == 1 || dashCount == 2)
            {
                if (tok.size() <= dashCount)
                {
                    // token is just '-' or '--', ignore
                    ++i;
                    continue;
                }

                String keyStr = String(StringView(tok.data() + dashCount, tok.size() - dashCount));

                // Determine value: use next token only if it's not a valid key token
                String value;
                if (i + 1 < tokens.size())
                {
                    const String& nextTok = tokens[i + 1];
                    if (!isValidKeyToken(nextTok))
                    {
                        value = trimQuotes(nextTok);
                    }
                }

                outOptions[std::string(keyStr.data(), keyStr.size())] = value;
                i += value.empty() ? 1 : 2;
            }
            else
            {
                // Not a key: bare token or token with >2 dashes, ignore
                ++i;
            }
        }
    }

    UnorderedMap<String, String> ProgramArguments::GetProgramArguments()
    {
        return CommandLineToProgramArguments(GetCommandLine());
    }

    UnorderedMap<String, String> ProgramArguments::CommandLineToProgramArguments(StringView cmdLine)
    {
        UnorderedMap<String, String> options;
        ParseArgsLine(cmdLine, options);
        return options;
    }
}

