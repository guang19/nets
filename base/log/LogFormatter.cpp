//
// Created by YangGuang on 2022/4/21.
//

#include "LogFormatter.h"

#include "base/ThreadHelper.h"

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
			__thread ::std::time_t CacheSeconds { 0 };
		}

		void DefaultLogFormatter::formatLogMessage(LogBuffer& logBufferStream, LogMessage& logMessage)
		{
			struct tm tmS {};
			const LogMessageTime& logMessageTime = logMessage.getLogMessageTime();
			::std::time_t seconds = logMessageTime.getSeconds();
			if (seconds != CacheSeconds)
			{
				if (localtime_r(&seconds, &tmS) == nullptr)
				{
					memset(&tmS, 0, sizeof(tmS));
				}
				CacheSeconds  = seconds;
				CacheTMS = tmS;
			}
			else
			{
				tmS = CacheTMS;
			}
			::std::snprintf(logBufferStream.getCurrentBuffer(), 24,
					 "%04d-%02d-%02d %02d:%02d:%02d.%03d",
					 tmS.tm_year + 1900, tmS.tm_mon + 1, tmS.tm_mday, tmS.tm_hour, tmS.tm_min, tmS.tm_sec,
					 logMessageTime.getMicroseconds());
			logBufferStream.addLen(23);
			logBufferStream << " [" << currentTid() << "] ";
			logBufferStream << LogLevelName[logMessage.getLogLevel()] << ' ';
			logBufferStream << logMessage.getFilename() << ':' << logMessage.getLine() << " - ";
			logBufferStream << logMessage.getMessage();
		}

		ILogFormatter *LogFormatterFactory::getLogFormatter() const
		{
			return DefaultLogFormatter::getInstance();
		}
	} // namespace base
} // namespace nets