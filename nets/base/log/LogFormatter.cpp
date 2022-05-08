//
// Created by YangGuang on 2022/4/21.
//

#include "nets/base/log/LogFormatter.h"

#include <cstring>
#include "nets/base/ThreadHelper.h"

namespace nets
{
	namespace base
	{
		INIT_SINGLETON(DefaultLogFormatter);
		INIT_SINGLETON(LogFormatterFactory);

		namespace
		{
			const char* const LogLevelName[LogLevel::NUM_OF_LOG_LEVELS] =
				{
					"TRACE",
					"DEBUG",
					"INFO",
					"WARN",
					"ERROR",
					"FATAL"
				};

			/**
			 * log time cache
			 */
			__thread struct tm CacheTMS {};
			__thread ::time_t CacheSeconds { 0 };
		}

		void DefaultLogFormatter::formatLogMessage(LogBuffer& logBuffer, LogMessage& logMessage)
		{
			struct tm tmS {};
			const Timestamp& logTime = logMessage.getLogTime();
			::time_t seconds = logTime.getSecondsSinceEpoch();
			if (seconds != CacheSeconds)
			{
				if (localtime_r(&seconds, &tmS) == nullptr)
				{
					::memset(&tmS, 0, sizeof(tmS));
				}
				CacheSeconds  = seconds;
				CacheTMS = tmS;
			}
			else
			{
				tmS = CacheTMS;
			}
			char timeBuf[24] = { 0 };
			::snprintf(timeBuf, 24, "%04d-%02d-%02d %02d:%02d:%02d.%03d", tmS.tm_year + 1900,
				tmS.tm_mon + 1, tmS.tm_mday, tmS.tm_hour, tmS.tm_min, tmS.tm_sec, logTime.getMicroseconds());
			logBuffer << timeBuf;
			logBuffer << " [" << currentTid() << "] ";
			logBuffer << LogLevelName[logMessage.getLogLevel()] << ' ';
			logBuffer << logMessage.getFilename() << ':' << logMessage.getLine() << " - ";
			logBuffer << logMessage.getStream();
		}

		ILogFormatter* LogFormatterFactory::getLogFormatter() const
		{
			return DefaultLogFormatter::getInstance();
		}
	} // namespace base
} // namespace nets