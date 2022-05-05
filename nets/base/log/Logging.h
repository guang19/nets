//
// Created by YangGuang on 2022/4/1.
//

#ifndef NETS_LOGGING_H
#define NETS_LOGGING_H


#include <cstring>
#include "nets/base/Copyable.h"
#include "nets/base/log/LogBuffer.h"
#include "nets/base/Noncopyable.h"
#include "nets/base/Timestamp.h"

#ifndef LOG_LEVEL
#define LOG_LEVEL 1
#endif

namespace nets
{
	namespace base
	{
		enum LogLevel
		{
			TRACE = 0,
			DEBUG,
			INFO,
			WARN,
			ERROR,
			FATAL,
			NUM_OF_LOG_LEVELS = 6
		};

		typedef struct LogMessage_ : Noncopyable
		{
			public:
				explicit LogMessage_(LogLevel logLevel, const char* file, uint32_t line);
				~LogMessage_() = default;

			public:
				inline const Timestamp& getLogTime() const
				{
					return logTime_;
				}

				inline LogLevel getLogLevel() const
				{
					return logLevel_;
				}

				inline const char* getFilename() const
				{
					return filename_;
				}

				inline uint32_t getLine() const
				{
					return line_;
				}

				inline LogBuffer& getMessage()
				{
					return message_;
				}

			private:
				Timestamp logTime_ {};
				LogLevel logLevel_ {};
				const char* filename_ { nullptr };
				uint32_t line_ { 0 };
				LogBuffer message_ {};
		} LogMessage;

		class LogMessageStream : public LogBuffer
		{
			public:
				explicit LogMessageStream(LogLevel logLevel, const char* file, uint32_t line);
				explicit LogMessageStream(LogLevel logLevel, const char* file, uint32_t line, const char* fmt, ...);
				~LogMessageStream();

			public:
				inline LogBuffer& stream()
				{
					return logMessage_.getMessage();
				}

			private:
				LogMessage logMessage_;
		};
	} // namespace base
} // namespace nets

#define LOG_TRACE(fmt, ...)	\
	do	\
	{	\
		if (LogLevel::TRACE >= LOG_LEVEL)	\
		{	\
			nets::base::LogMessageStream(LogLevel::TRACE, __FILE__, __LINE__, fmt, ##__VA_ARGS__);	\
		}	\
	}	\
	while (0)

#define LOG_DEBUG(fmt, ...)	\
	do	\
	{	\
		if (LogLevel::DEBUG >= LOG_LEVEL)	\
		{	\
			nets::base::LogMessageStream(LogLevel::DEBUG, __FILE__, __LINE__, fmt, ##__VA_ARGS__);	\
		}	\
	}	\
	while (0)

#define LOG_INFO(fmt, ...)	\
	do	\
	{	\
		if (LogLevel::INFO >= LOG_LEVEL)	\
		{	\
			nets::base::LogMessageStream(LogLevel::INFO, __FILE__, __LINE__, fmt, ##__VA_ARGS__);	\
		}	\
	}	\
	while (0)

#define LOG_WARN(fmt, ...)	\
	do	\
	{	if (LogLevel::WARN >= LOG_LEVEL)	\
		{	\
			nets::base::LogMessageStream(LogLevel::WARN, __FILE__, __LINE__, fmt, ##__VA_ARGS__);	\
		}	\
	}	\
	while (0)

#define LOG_ERROR(fmt, ...)	\
	do	\
	{	\
		nets::base::LogMessageStream(LogLevel::ERROR, __FILE__, __LINE__, fmt, ##__VA_ARGS__);	\
	}	\
	while (0)

#define LOG_FATAL(fmt, ...)	\
	do	\
	{	\
		nets::base::LogMessageStream(LogLevel::FATAL, __FILE__, __LINE__, fmt, ##__VA_ARGS__);	\
	}	\
	while (0)

// stream api
#define LOGS_TRACE	\
		if (LogLevel::TRACE >= LOG_LEVEL)	\
			nets::base::LogMessageStream(LogLevel::TRACE, __FILE__, __LINE__).stream()

#define LOGS_DEBUG	\
		if (LogLevel::DEBUG >= LOG_LEVEL)	\
			nets::base::LogMessageStream(LogLevel::DEBUG, __FILE__, __LINE__).stream()

#define LOGS_INFO	\
		if (LogLevel::INFO >= LOG_LEVEL)	\
			nets::base::LogMessageStream(LogLevel::INFO, __FILE__, __LINE__).stream()

#define LOGS_WARN	\
		if (LogLevel::WARN >= LOG_LEVEL)	\
			nets::base::LogMessageStream(LogLevel::WARN, __FILE__, __LINE__).stream()

#define LOGS_ERROR	\
			nets::base::LogMessageStream(LogLevel::ERROR, __FILE__, __LINE__).stream()

#define LOGS_FATAL	\
			nets::base::LogMessageStream(LogLevel::FATAL, __FILE__, __LINE__).stream()

#endif //NETS_LOGGING_H
