//
// Created by YangGuang on 2022/4/1.
//

#include "nets/base/log/Logging.h"

#include <cstdarg>
#include "nets/base/log/LogWriter.h"
#include "nets/base/log/LogFormatter.h"

namespace nets
{
	namespace base
	{
		LogMessage_::LogMessage_(LogLevel logLevel, const char* file, uint32_t line) :
			logTime_(Timestamp::now()), logLevel_(logLevel), line_(line), message_()
		{
			filename_ = basename(file);
		}

		LogMessageStream::LogMessageStream(LogLevel logLevel, const char* file, uint32_t line) :
			logMessage_(logLevel, file, line)
		{
		}

		LogMessageStream::LogMessageStream(LogLevel logLevel, const char* file, uint32_t line, const char* fmt, ...) :
			logMessage_(logLevel, file, line)
		{
			va_list args;
			va_start(args, fmt);
			va_list tmp;
			va_copy(tmp, args);
			uint32_t len = ::std::vsnprintf(nullptr, 0, fmt, tmp);
			va_end(tmp);
			LogBuffer& msg = logMessage_.getMessage();
			if (len > 0)
			{
				if (available() > len)
				{
					::std::vsnprintf(msg.getCurrentBuffer(), len + 1, fmt, args);
					msg.addLen(len);
					msg << '\n';
				}
				else
				{
					msg << "[Log content length more than LogBufferSize]\n";
				}
			}
			va_end(args);
		}

		LogMessageStream::~LogMessageStream()
		{
			LOG_FORMATTER->formatLogMessage(*this, logMessage_);
			LOG_WRITER->write(getBuffer(), length());
		}
	} // namespace base
} // namespace nets
