//
// Created by YangGuang on 2022/4/1.
//

#ifndef NETS_LOGGING_H
#define NETS_LOGGING_H


#include "nets/base/log/LogMessage.h"

#ifndef LOG_LEVEL
#define LOG_LEVEL 1
#endif

namespace nets
{
	namespace base
	{
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
