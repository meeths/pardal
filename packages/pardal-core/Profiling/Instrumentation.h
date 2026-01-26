
#pragma once

// Created on 2026-01-23 by sisco
#ifdef PDL_FEATURE_TRACY
#include <Tracy/Tracy.hpp>

#define pdlProfilerInit() TracyNoop

#define pdlProfileScoped() ZoneScoped
#define pdlProfileScopedN(name) ZoneScopedN(name)
#define pdlProfileAddText(text, size) ZoneText(text, size)

#define pdlFrameMark() FrameMark
#define pdlFrameMarkN(name) FrameMarkNamed(name)

#define pdlProfileLockable(type, name) TracyLockable(type, name)

#define pdlProfileAlloc(ptr, size) TracyAlloc(ptr, size)
#define pdlProfileFree(ptr) TracyFree(ptr)
#define pdlProfileAllocN(ptr, size) TracyAllocN(ptr, size)
#define pdlProfileFreeN(ptr) TracyFreeN(ptr)

#define pdlPlot( name, val ) TracyPlot( name, val )
#define pdlPlotConfig( name, type, step, fill, color ) TracyPlotConfig( name, type, step, fill, color )

#else

#define pdlProfilerInit()

#define pdlProfileScoped()
#define pdlProfileScopedN(name)
#define pdlProfileAddText(text, size)

#define pdlFrameMark()
#define pdlFrameMarkN(name)

#define pdlProfileLockable(type, name)

#define pdlProfileAlloc(ptr, size)
#define pdlProfileFree(ptr)
#define pdlProfileAllocN(ptr, size)
#define pdlProfileFreeN(ptr)

#define pdlPlot( name, val )
#define pdlPlotConfig( name, type, step, fill, color )


#endif