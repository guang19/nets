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
			return LogMessageTime(tmV.tv_sec, static_cast<uint32_t>(tmV.tv_usec));
		}

		uint64_t LogMessageTime::getTimestamp() const
		{
			return (secondsEpoch_ * MicrosecondsPerSecond) + microseconds_;
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

		LogMessage_::LogMessage_(const LogMessageTime& time, LogLevel logLevel, const char* file, uint32_t line) noexcept :
			logMessageTime_(time), logLevel_(logLevel), line_(line), logBufferStream_()
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

		void LogMessage::formatMessageInfo()
		{
//			logBufferStream_ << logMessageTime_;
			logBufferStream_ << " [" << static_cast<int64_t>(currentTid()) << "] ";
			logBufferStream_ << LogLevelName[logLevel_] << ' ';
			logBufferStream_ << filename_ << ':' << line_ << " - ";
		}

		void LogMessage::formatMessageArgs(const char* fmt, va_list args)
		{
			va_list tmp;
			va_copy(tmp, args);
			uint32_t len = vsnprintf(nullptr, 0, fmt, tmp) + 1;
			va_end(tmp);
			if (len > 0 && logBufferStream_.available() > len)
			{
				vsnprintf(logBufferStream_.getCurrentBuffer(), len, fmt, args);
				logBufferStream_.addLen(len - 1);
				logBufferStream_ << '\n';
			}
			else
			{
				logBufferStream_ << "[Log content length more than LogBufferSize]\n";
			}
		}

		Logger::Logger(LogLevel logLevel, const char* file, uint32_t line) noexcept :
			logMessage_(LogMessageTime::now(), logLevel, file, line)
		{
			logMessage_.formatMessageInfo();
		}

		Logger::Logger(LogLevel logLevel, const char* file, uint32_t line, const char* fmt, ...) noexcept :
			logMessage_(LogMessageTime::now(), logLevel, file, line)
		{
			logMessage_.formatMessageInfo();
			va_list args;
			va_start(args, fmt);
			logMessage_.formatMessageArgs(fmt, args);
			va_end(args);
		}

		Logger::~Logger()
		{
			fwrite(getMessageStream().getBuffer(), 1, getMessageStream().length(), ::stdout);
			fflush(::stdout);
		}
	} // namespace base
} // namespace nets
