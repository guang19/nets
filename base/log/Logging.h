//
// Created by YangGuang on 2022/4/1.
//

#ifndef NETS_LOGGING_H
#define NETS_LOGGING_H

#include <string>
#include <string.h>
#include "base/Noncopyable.h"
#include "base/ThreadHelper.h"
#include "base/log/LogBuffer.h"
#include "base/log/LoggerManager.h"

namespace nets
{
	namespace base
	{
		typedef struct LogLineComponent_
		{
			public:
				Timestamp time_;
				LogLevel logLevel_;
				const char* sourcefile_;
//				const char* function_;
				uint32_t line_;

				LogLineComponent_(const Timestamp& time, LogLevel logLevel,
										   const char* sourcefile, uint32_t line) :
					time_(time), logLevel_(logLevel), line_(line)
				{
					const char* basename = strrchr(sourcefile, '/');
					if (basename != nullptr)
					{
						sourcefile_ = basename + 1;
					}
					else
					{
						sourcefile_ = sourcefile;
					}
				}
		} LogLineComponent;

		class Logger
		{
			public:
				Logger(LogLevel logLevel, const char* sourcefile, uint32_t line);
				~Logger();

			public:
				void log(const char* fmt, ...);

				inline LogBuffer& getLogStream()
				{
					return logBuffer_;
				}

			private:
				void recordLineComponent(const LogLineComponent& logLineComponent);
				void recordFormat(const char* fmt, va_list args);

			private:
				LogBuffer logBuffer_ {};
				LogLineComponent logLineComponent_;
		};
	} // namespace base
} // namespace nets

#define LOG_TRACE(fmt, ...) \
	do                         \
	{                          \
        if (LogLevel::TRACE >= LogMgr->getLogLevel()) \
		{	\
			nets::base::Logger(LogLevel::TRACE, __FILE__, __LINE__).log(fmt, ##__VA_ARGS__);	\
		}	\
	}                          \
	while (0);

#define LOG_DEBUG(fmt, ...) \
	do                         \
	{                          \
		if (LogLevel::DEBUG >= LogMgr->getLogLevel()) \
		{	\
			nets::base::Logger(LogLevel::DEBUG, __FILE__, __LINE__).log(fmt, ##__VA_ARGS__);	\
		}	\
	}                          \
	while (0);

#define LOG_INFO(fmt, ...) \
	do                         \
	{                          \
		if (LogLevel::INFO >= LogMgr->getLogLevel()) \
		{	\
			nets::base::Logger(LogLevel::INFO, __FILE__, __LINE__).log(fmt, ##__VA_ARGS__);	\
		}	\
	}                          \
	while (0);

#define LOG_WARN(fmt, ...) \
	do                         \
	{                          \
		nets::base::Logger(LogLevel::WARN, __FILE__, __LINE__).log(fmt, ##__VA_ARGS__);	\
	}                          \
	while (0);

#define LOG_ERROR(fmt, ...) \
	do                         \
	{                          \
		nets::base::Logger(LogLevel::ERROR, __FILE__, __LINE__).log(fmt, ##__VA_ARGS__);	\
	}                          \
	while (0);

#define LOG_FATAL(fmt, ...) \
	do                         \
	{                          \
		nets::base::Logger(LogLevel::FATAL, __FILE__, __LINE__).log(fmt, ##__VA_ARGS__);	\
	}                      	\
	while (0);


// stream api

#define LOGS_TRACE \
		if (LogLevel::TRACE >= LogMgr->getLogLevel()) \
			nets::base::Logger(LogLevel::TRACE, __FILE__, __LINE__).getLogStream()

#define LOGS_DEBUG \
		if (LogLevel::DEBUG >= LogMgr->getLogLevel()) \
			nets::base::Logger(LogLevel::DEBUG, __FILE__, __LINE__).getLogStream()

#define LOGS_INFO \
		if (LogLevel::INFO >= LogMgr->getLogLevel()) \
			nets::base::Logger(LogLevel::INFO, __FILE__, __LINE__).getLogStream()

#define LOGS_WARN \
		if (LogLevel::WARN >= LogMgr->getLogLevel()) \
			nets::base::Logger(LogLevel::WARN, __FILE__, __LINE__).getLogStream()

#define LOGS_ERROR \
		if (LogLevel::ERROR >= LogMgr->getLogLevel()) \
			nets::base::Logger(LogLevel::ERROR, __FILE__, __LINE__).getLogStream()

#define LOGS_FATAL \
		if (LogLevel::FATAL >= LogMgr->getLogLevel()) \
			nets::base::Logger(LogLevel::FATAL, __FILE__, __LINE__).getLogStream()

#endif //NETS_LOGGING_H
