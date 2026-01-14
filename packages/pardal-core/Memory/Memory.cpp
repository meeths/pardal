
#include <Memory/Memory.h>
#include "mimalloc-new-delete.h"
#include "Base/DebugHelpers.h"
// Created on 2025-12-01 by Sisco

namespace pdl
{
    void Memory::Initialize()
    {
#ifndef PDL_RELEASE
        mi_option_enable(mi_option_show_stats);
        mi_option_enable(mi_option_show_errors);
        mi_option_enable(mi_option_verbose);
#endif  
        
    }
}

void* operator new[](size_t size, const char* pName, int flags, unsigned debugFlags, const char* file, int line)
{
    return ::operator new[](size);
}
void* operator new[](size_t size, size_t alignment, size_t alignmentOffset, const char* pName, int flags, unsigned debugFlags, const char* file, int line)
{
    pdlAssert(alignmentOffset == 0);
    return ::operator new[](size, static_cast<std::align_val_t>(alignment));
}
