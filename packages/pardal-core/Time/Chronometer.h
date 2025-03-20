#pragma once
#include <chrono>
#include <Time/TimeTypes.h>

namespace pdl
{

template <typename ClockType = TimeTypes::SteadyClock>
class Chronometer
{
public:
	void Start();
	void Stop();

	template <typename ValueType, typename TimeType>
	ValueType Lap();
	void Reset();
	template <typename ValueType, typename TimeType>
	ValueType GetElapsed();
private:
	std::chrono::time_point<ClockType> mStartTimepoint;
	std::chrono::time_point<ClockType> mLastLapTimepoint;
	bool mStarted = false;

};

template <typename ClockType>
void Chronometer<ClockType>::Start()
{
	if(mStarted)
		return;

	mStarted = true;
	mStartTimepoint = ClockType::now();
	mLastLapTimepoint = ClockType::now();
}

template <typename ClockType>
void Chronometer<ClockType>::Stop()
{
	if(!mStarted)
		return;
	
	mStarted = false;
	mLastLapTimepoint = ClockType::now();
}

template <typename ClockType>
template <typename ValueType, typename TimeType>
ValueType Chronometer<ClockType>::Lap()
{
	auto nowTimepoint = ClockType::now();
	std::chrono::duration<ValueType, TimeType> value = nowTimepoint - mLastLapTimepoint;
	mLastLapTimepoint = nowTimepoint;
	return value.count();
}

template <typename ClockType>
void Chronometer<ClockType>::Reset()
{
	if(!mStarted)
		return;
	mStarted = false;
	Start();
}

template <typename ClockType>
template <typename ValueType, typename TimeType>
ValueType Chronometer<ClockType>::GetElapsed()
{
	auto nowTimepoint = ClockType::now();
	std::chrono::duration<ValueType, TimeType> value = nowTimepoint - mStartTimepoint;
	return value.count();
}

}
