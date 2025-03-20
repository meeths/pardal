
#include <Log/LoggerStdout.h>
#include <iostream>
#include <String/String.h>

// Created on 2020-08-13 by Sisco

#define RESET   "\033[0m"
#define BLACK   "\033[30m"              
#define RED     "\033[31m"              
#define GREEN   "\033[32m"              
#define YELLOW  "\033[33m"              
#define BLUE    "\033[34m"              
#define MAGENTA "\033[35m"              
#define CYAN    "\033[36m"              
#define WHITE   "\033[37m"              
#define BOLDBLACK   "\033[1m\033[30m"   
#define BOLDRED     "\033[1m\033[31m"   
#define BOLDGREEN   "\033[1m\033[32m"   
#define BOLDYELLOW  "\033[1m\033[33m"   
#define BOLDBLUE    "\033[1m\033[34m"   
#define BOLDMAGENTA "\033[1m\033[35m"   
#define BOLDCYAN    "\033[1m\033[36m"   
#define BOLDWHITE   "\033[1m\033[37m"   

namespace pdl
{
    void LoggerStdout::Log(LogType logType, const String& _log)
    {
        const char* color;
        switch (logType)
        {
        case LogType::LOG_ERROR: color = BOLDRED; break;
        case LogType::LOG_WARNING: color = BOLDYELLOW; break;
        case LogType::LOG_INFO:
        default: color = RESET;
        }

        std::cout << color << _log << RESET << '\n';
    }

    void LoggerStdout::Flush()
    {
        std::cout << std::flush;
    }
}

