//
// Created by YangGuang on 2022/4/1.
//

#include "base/log/Logging.h"
#include "base/log/LogBuffer.h"

namespace nets
{
	namespace base
	{
		namespace
		{
			constexpr uint32_t LogBufferFixedCapacity = 4096;
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

		void Logger::trace(const LogLineComponent& logLineComponent)
		{
			LogBuffer logBuffer(LogBufferFixedCapacity);
			logBuffer << logLineComponent.time_.formatTime() << ' ';
			currentTid();
			logBuffer << '[';
			logBuffer.append(tidString_, tidStringLength_);
			logBuffer << ']';
			logBuffer << ' ';
			logBuffer << '[';
			logBuffer << LogLevelName[logLineComponent.logLevel_];
			logBuffer << ']' << ' ';
			logBuffer << logLineComponent.sourcefile_;
			logBuffer << "::" << logLineComponent.function_ << ':';
			logBuffer << logLineComponent.line_ << ' ' << '-' << ' ';
			fwrite(logBuffer.getBuffer(),1, logBuffer.length(), ::stdout);
			fflush(::stdout);
		}
	} // namespace base
} // namespace nets
