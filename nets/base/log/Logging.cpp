//
// Created by YangGuang on 2022/4/1.
//

#include "nets/base/log/Logging.h"

#include "nets/base/log/LogFormatter.h"
#include "nets/base/log/LogWriter.h"

namespace nets::base
{
	LogMessageStream::LogMessageStream(LogLevel logLevel, const char* file, uint32_t line)
		: logMessage_(logLevel, file, line)
	{
	}

	LogMessageStream::~LogMessageStream()
	{
		logMessage_.getStream() << '\n';
		LOG_FORMATTER->formatLogMessage(*this, logMessage_);
		LOG_WRITER->write(buffer(), writerIndex());
		if (logMessage_.getLogLevel() == LogLevel::FATAL)
		{
			// if exit directly, log buffer in memory probably will lost
			::std::this_thread::sleep_for(::std::chrono::milliseconds(MillisecondsPerSecond));
			::fprintf(::stderr, "log fatal, exit");
			exit(1);
		}
	}
} // namespace nets::base
