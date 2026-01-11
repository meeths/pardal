
#pragma once

// Created on 2026-01-11 by Sisco

#include "Containers/UnorderedMap.h"
#include "String/String.h"

namespace pdl
{

class ProgramArguments
{
public:
    // Returns the current process command line as a UTF-8 pdl::String.
    // Platform-specific implementation is provided per OS; on unsupported
    // platforms this may return an empty string.
    static String GetCommandLine();
    static UnorderedMap<String, String> GetProgramArguments();
    static UnorderedMap<String, String> CommandLineToProgramArguments(StringView cmdLine);

private:
       // Parses a raw command line string into key-value options and stores them into outOptions.
    // Expected format: <exe> <key> <value> [<key> <"value with spaces">] ...
    // Keys may optionally start with '-' or '--'. Quotes around values are removed.
    static void ParseArgsLine(StringView cmdLine, UnorderedMap<String, String>& outOptions);
};

}

