//
// Created by YangGuang on 2022/4/1.
//

#include "base/log/Logging.h"

#include <cstdarg>
#include <sys/time.h>
#include "base/log/LogWriter.h"
#include "base/log/LogFormatter.h"

namespace nets
{
	namespace base
	{
		namespace
		{
			constexpr uint32_t MicrosecondsPerSecond = 1000000U;
		}

		LogMessageTime::LogMessageTime() : timestampEpoch_(0)
		{
		}

		LogMessageTime::LogMessageTime(uint64_t timestamp) : timestampEpoch_(timestamp)
		{
		}

		LogMessageTime::LogMessageTime(::std::time_t secondsEpoch, ::std::time_t microseconds)
			: LogMessageTime((secondsEpoch * MicrosecondsPerSecond) + microseconds)
		{
		}

		LogMessageTime::LogMessageTime(const LogMessageTime& rhs)
		{
			timestampEpoch_ = rhs.timestampEpoch_;
		}

		LogMessageTime::LogMessageTime(LogMessageTime&& rhs)
		{
			timestampEpoch_ = rhs.timestampEpoch_;
			rhs.timestampEpoch_ = 0;
		}

		LogMessageTime& LogMessageTime::operator=(const LogMessageTime& rhs)
		{
			if (this != &rhs)
			{
				timestampEpoch_ = rhs.timestampEpoch_;
			}
			return *this;
		}

		LogMessageTime& LogMessageTime::operator=(LogMessageTime&& rhs)
		{
			if (this != &rhs)
			{
				timestampEpoch_ = rhs.timestampEpoch_;
				rhs.timestampEpoch_ = 0;
			}
			return *this;
		}

		LogMessageTime LogMessageTime::now()
		{
			struct timeval tmV {};
			::gettimeofday(&tmV, nullptr);
			return { tmV.tv_sec, tmV.tv_usec };
		}

		::std::time_t LogMessageTime::getSeconds() const
		{
			return static_cast<::std::time_t>(timestampEpoch_ / MicrosecondsPerSecond);
		}

		uint32_t LogMessageTime::getMicroseconds() const
		{
			return timestampEpoch_ % MicrosecondsPerSecond;
		}

		LogMessage_::LogMessage_(LogLevel logLevel, const char* file, uint32_t line) :
			logMessageTime_(LogMessageTime::now()), logLevel_(logLevel), line_(line), message_()
		{
			setFilenameFromPath(file);
		}

		void LogMessage_::setFilenameFromPath(const char* file)
		{
			const char* basename = strrchr(file, '/');
			if (basename != nullptr)
			{
				filename_ = basename + 1;
			}
			else
			{
				filename_ = file;
			}
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
			uint32_t len = ::std::vsnprintf(nullptr, 0, fmt, tmp) + 1;
			va_end(tmp);
			LogBuffer& msg = logMessage_.getMessage();
			if (len > 0 && available() > len)
			{
				::std::vsnprintf(msg.getCurrentBuffer(), len, fmt, args);
				msg.addLen(len - 1);
				msg << '\n';
			}
			else
			{
				msg << "[Log content length more than LogBufferSize]\n";
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
