
#include <Memory/Memory.h>
#include "mimalloc-new-delete.h"
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

