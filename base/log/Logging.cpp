//
// Created by YangGuang on 2022/4/1.
//

#include "base/log/Logging.h"

#include <stdarg.h>

namespace nets
{
	namespace base
	{
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
		}

		Logger::Logger(LogLevel logLevel, const char* sourcefile, uint32_t line) : logBuffer_(),
		logLineComponent_(Timestamp::now(), logLevel, sourcefile, line)
		{
			recordLineComponent(logLineComponent_);
		}

		Logger::~Logger()
		{
			LogMgr->getLogAppender()->append(logBuffer_);
		}

		void Logger::log(const char* fmt, ...)
		{
			va_list args;
			va_start(args, fmt);
			recordFormat(fmt, args);
			va_end(args);
		}

		void Logger::recordLineComponent(const LogLineComponent& logLineComponent)
		{
			logBuffer_ << logLineComponent.time_;
			logBuffer_ << " [" << static_cast<int32_t>(currentTid()) << "] ";
			logBuffer_ << LogLevelName[logLineComponent.logLevel_] << ' ';
			logBuffer_ << logLineComponent.sourcefile_ << ':' << logLineComponent.line_ << " - ";
		}

		void Logger::recordFormat(const char* fmt, va_list args)
		{
			va_list tmp;
			va_copy(tmp, args);
			uint32_t len = vsnprintf(nullptr, 0, fmt, tmp) + 1;
			va_end(tmp);
			if (len > 0 && logBuffer_.available() > len)
			{
				vsnprintf(logBuffer_.getCurrentBuffer(), len, fmt, args);
				logBuffer_.addLen(len - 1);
				logBuffer_ << '\n';
			}
			else
			{
				logBuffer_ << "[Log content length more than LOG_BUFFER_SIZE]\n";
			}
		}
	} // namespace base
} // namespace nets
