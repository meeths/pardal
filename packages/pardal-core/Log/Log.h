#pragma once
#include <Base/ServiceLocator.h>
#include <Log/ILogger.h>
#include <Memory/SharedPointer.h>
#include <String/String.h>
#include <Containers/Vector.h>
#include <Threading/SRWLock.h>

#define pdlLogError(x, ...) pdl::ServiceLocator<pdl::Log>::Ref().LogError(x, __VA_ARGS__)
#define pdlLogWarning(x, ...) pdl::ServiceLocator<pdl::Log>::Ref().LogWarning(x, __VA_ARGS__)
#define pdlLogInfo(x, ...) pdl::ServiceLocator<pdl::Log>::Ref().LogInfo(x, __VA_ARGS__)
#define pdlLogFlush() pdl::ServiceLocator<pdl::Log>::Ref().Flush()

namespace pdl
{
class ILogger;
	
class Log
{
public:

	void RegisterLogger(SharedPointer<ILogger> _logger);
	
	void LogWarning(StringView fmt, ...) ;
	void LogError(StringView fmt, ...) ;
	void LogInfo(StringView fmt, ...) ;

	void Flush();
private:
	
	void LogDetail(LogType logType, StringView fmt, va_list args);
	
	Vector<SharedPointer<ILogger>> m_Loggers;
	SRWLock m_loggersLock;
};
}