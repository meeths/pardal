
#pragma once
#include <Log/ILogger.h>
#include <String/String.h>

// Created on 2020-08-13 by Sisco

namespace pdl
{

class LoggerStdout : public ILogger
{
public:
    ~LoggerStdout() override = default;
    void Log(LogType logType, const String& _log) override;
    void Flush() override;
};

}

