
#pragma once
#include "Base/Expected.h"
#include "String/String.h"

// Created on 2026-03-01 by Sisco

namespace pdl
{

namespace CoreSystems
{
    Expected<void, StringView> Initialize();
    bool IsInitialized();
};

}

