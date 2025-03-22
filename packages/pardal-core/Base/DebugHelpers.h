
#pragma once

// Created on 2025-03-19 by sisco
#ifndef NDEBUG
    #include <cassert>
    #define pdlAssert(x) assert(x)
    #define pdlNotImplemented() assert(false && "Not implemented")
#else
    #define pdlAssert(x) do { (void)sizeof(x);} while (0)
    #define pdlNotImplemented() do { } while (0)
#endif
