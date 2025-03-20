#pragma once
#include <ratio>
#include <chrono>

namespace pdl
{

namespace TimeTypes
{
	typedef std::nano Nanoseconds;
	typedef std::micro Microseconds;
	typedef std::milli Milliseconds;
	typedef std::ratio<1, 1> Seconds;

	typedef std::chrono::steady_clock SteadyClock;
	typedef std::chrono::system_clock SystemClock;
	typedef std::chrono::high_resolution_clock HighResolutionClock;

};

}
