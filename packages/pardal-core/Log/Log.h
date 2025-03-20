#pragma once
#include <Base/ISingleton.h>
#include <Log/ILogger.h>
#include <Memory/SharedPointer.h>
#include <String/String.h>
#include <Containers/Vector.h>
#include <Threading/SRWLock.h>

#define pdlLogError(x, ...) pdl::Log::Instance().LogError(x, __VA_ARGS__)
#define pdlLogWarning(x, ...) pdl::Log::Instance().LogWarning(x, __VA_ARGS__)
#define pdlLogInfo(x, ...) pdl::Log::Instance().LogInfo(x, __VA_ARGS__)
#define pdlLogFlush() pdl::Log::Instance().Flush()

namespace pdl
{
class ILogger;
	
class Log : public ISingleton<Log>
{
public:

	void RegisterLogger(SharedPointer<ILogger> _logger);
	
	void LogWarning(const char* fmt, ...) ;
	void LogError(const char* fmt, ...) ;
	void LogInfo(const char* fmt, ...) ;

	void Flush();
private:
	
	void LogDetail(LogType logType, const char* fmt, va_list args);
	
	Vector<SharedPointer<ILogger>> m_Loggers;
	SRWLock m_loggersLock;
};
}