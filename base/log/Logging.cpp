//
// Created by YangGuang on 2022/4/1.
//

#include "base/log/Logging.h"
#include <cstdarg>
#include <sys/time.h>
#include "base/ThreadHelper.h"

namespace nets
{
	namespace base
	{
		namespace
		{
			constexpr uint32_t MicrosecondsPerSecond = 1000000U;
		}

		LogMessageTime::LogMessageTime() : timeS_(), secondsEpoch_(0), microseconds_(0)
		{
		}

		LogMessageTime::LogMessageTime(uint64_t timestamp)
		{
			secondsEpoch_ = static_cast<::std::time_t>(timestamp / MicrosecondsPerSecond);
			microseconds_ = timestamp % MicrosecondsPerSecond;
			localtime_r(&secondsEpoch_, &timeS_);
		}

		LogMessageTime::LogMessageTime(::std::time_t secondsEpoch, uint32_t microseconds)
		{
			localtime_r(&secondsEpoch, &timeS_);

		}

		LogMessageTime::LogMessageTime(const LogMessageTime& rhs)
		{
			timeS_ = rhs.timeS_;
			secondsEpoch_ = rhs.secondsEpoch_;
			microseconds_ = rhs.microseconds_;
		}

		LogMessageTime::LogMessageTime(LogMessageTime&& rhs) noexcept
		{
			timeS_ = rhs.timeS_;
			secondsEpoch_ = rhs.secondsEpoch_;
			microseconds_ = rhs.microseconds_;
			rhs.timeS_ = {};
			rhs.secondsEpoch_ = 0;
			rhs.microseconds_ = 0;
		}

		LogMessageTime& LogMessageTime::operator=(const LogMessageTime& rhs)
		{
			if (this != &rhs)
			{
				timeS_ = rhs.timeS_;
				secondsEpoch_ = rhs.secondsEpoch_;
				microseconds_ = rhs.microseconds_;
			}
			return *this;
		}

		LogMessageTime& LogMessageTime::operator=(LogMessageTime&& rhs) noexcept
		{
			if (this != &rhs)
			{
				timeS_ = rhs.timeS_;
				secondsEpoch_ = rhs.secondsEpoch_;
				microseconds_ = rhs.microseconds_;
				rhs.timeS_ = {};
				rhs.secondsEpoch_ = 0;
				rhs.microseconds_ = 0;
			}
			return *this;
		}

		LogMessageTime LogMessageTime::now() noexcept
		{
			struct timeval tmV {};
			gettimeofday(&tmV, nullptr);
			return { tmV.tv_sec, static_cast<uint32_t>(tmV.tv_usec) };
		}

		uint64_t LogMessageTime::getTimestamp() const
		{
			return (secondsEpoch_ * MicrosecondsPerSecond) + microseconds_;
		}

		LogMessage_::LogMessage_(LogLevel logLevel, const char* file, uint32_t line) noexcept :
			logLevel_(logLevel), line_(line), message_(nullptr)
		{
			setFilenameFromPath(file);
		}

		LogMessage_::LogMessage_(LogLevel logLevel, const char* file, uint32_t line, const char* message) noexcept :
			logLevel_(logLevel), line_(line), message_(message)
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

		LogMessageStream::LogMessageStream(LogLevel logLevel, const char* file, uint32_t line) noexcept :
			logMessage_(logLevel, file, line)
		{
		}

		LogMessageStream::LogMessageStream(LogLevel logLevel, const char* file, uint32_t line, const char* fmt, ...) noexcept :
			logMessage_(logLevel, file, line)
		{
			va_list args;
			va_start(args, fmt);
			va_list tmp;
			va_copy(tmp, args);
			uint32_t len = vsnprintf(nullptr, 0, fmt, tmp) + 1;
			va_end(tmp);
			if (len > 0 && available() > len)
			{
				vsnprintf(getCurrentBuffer(), len, fmt, args);
				addLen(len - 1);
				this->operator<<('\n');
			}
			else
			{
				this->operator<<("[Log content length more than LogBufferSize]\n");
			}
			va_end(args);
		}

		LogMessageStream::~LogMessageStream()
		{
			//			logBufferStream_ << logMessageTime_;
//			logBufferStream_ << " [" << static_cast<int64_t>(currentTid()) << "] ";
//			logBufferStream_ << LogLevelName[logLevel_] << ' ';
//			logBufferStream_ << filename_ << ':' << line_ << " - ";
		}

		uint32_t DefaultLogFormatter::formatMessage(char *buffer, const LogMessage &logMessage)
		{
			return 0;
		}
	} // namespace base
} // namespace nets
