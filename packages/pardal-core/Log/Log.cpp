
#include <Log/Log.h>

// Created on 2020-04-30 by sisco
#include <cstdarg>
#include <Time/Chronometer.h>
#include <String/StringUtils.h>
#include <Containers/Array.h>

namespace pdl
{
	void Log::RegisterLogger(SharedPointer<ILogger> _logger)
	{
		SRWScopedWriteLock lock(m_loggersLock);
		m_Loggers.emplace_back(std::move(_logger));
	}

	void Log::LogWarning(const char* fmt, ...)
	{
		va_list args;
		va_start(args, fmt);
		LogDetail(LogType::LOG_WARNING, fmt, args);
		va_end(args);
	}

	void Log::LogError(const char* fmt, ...)
	{
		va_list args;
		va_start(args, fmt);
		LogDetail(LogType::LOG_ERROR, fmt, args);
		va_end(args);
	}

	void Log::LogInfo(const char* fmt, ...)
	{
		va_list args;
		va_start(args, fmt);
		LogDetail(LogType::LOG_INFO, fmt, args);
		va_end(args);
	}

	void Log::Flush()
	{
		for (auto& logger : m_Loggers)
		{
			logger->Flush();
		}
	}

	void Log::LogDetail(LogType logType, const char* fmt, va_list args)
	{
		SRWScopedReadLock lock(m_loggersLock);
		
		Array<char, 2048> buffer;

		vsnprintf(buffer.data(), buffer.size(), fmt, args);

		for (auto& logger : m_Loggers)
		{
			logger->Log(logType, String(buffer.data()));
		}
	}
	
}
