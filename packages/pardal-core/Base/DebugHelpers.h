
#pragma once

// Created on 2025-03-19 by sisco
#ifndef NDEBUG
    #include <cassert>
    #define pdlAssert(x) assert(x)
#else
    #define pdlAssert(x) do { (void)sizeof(x);} while (0)
#endif
