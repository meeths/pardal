
#pragma once
#include "EASTL/internal/atomic/atomic.h"

// Created on 2026-01-23 by sisco

namespace pdl
{   
    template <typename T>
    using Atomic = eastl::atomic<T>;
}

