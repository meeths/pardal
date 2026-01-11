
#pragma once

// Created on 2026-01-11 by Sisco

namespace pdl
{
class EngineOptions;
class Engine
{
public:
    Engine(const EngineOptions& options);
    void Run();
    ~Engine();
};

}

