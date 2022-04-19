//
// Created by YangGuang on 2022/4/1.
//

#ifndef NETS_LOGGING_H
#define NETS_LOGGING_H

#include <ctime>
#include <cstring>
#include <string>
#include "base/Noncopyable.h"
#include "base/log/LogBufferStream.h"

#ifndef LOG_LEVEL
	#define LOG_LEVEL DEBUG
#endif

#ifndef LOG_APPENDER
	#define LOG_APPENDER STDOUT
#endif

#ifndef LOG_PATH
	#define LOG_PATH "~/nets/log"
#endif

#ifndef LOG_ROLLING_SIZE
	#define LOG_ROLLING_SIZE 30
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

		class LogMessageTime
		{
			public:
				LogMessageTime();
				explicit LogMessageTime(uint64_t timestamp);
				LogMessageTime(::std::time_t secondsEpoch, uint32_t microseconds);

				LogMessageTime(const LogMessageTime& rhs);
				LogMessageTime(LogMessageTime&& rhs) noexcept;

				LogMessageTime& operator=(const LogMessageTime& rhs);
				LogMessageTime& operator=(LogMessageTime&& rhs) noexcept;

				static LogMessageTime now() noexcept;

				const struct tm& getTimeS() const
				{
					return timeS_;
				}

				uint64_t getTimestamp() const;

				::std::time_t getSecondsEpoch() const
				{
					return secondsEpoch_;
				}

			private:
				struct tm timeS_ {};
				::std::time_t secondsEpoch_ { 0 };
				uint32_t microseconds_ { 0 };
		};

		typedef struct LogMessage_
		{
			public:
				explicit LogMessage_(const LogMessageTime& time, LogLevel logLevel,
								  const char* file, uint32_t line) noexcept;

				inline LogBufferStream& getLogBufferStream()
				{
					return logBufferStream_;
				}

			public:
				void formatMessageInfo();
				void formatMessageArgs(const char* fmt, va_list args);

			private:
				LogMessageTime logMessageTime_ {};
				LogLevel logLevel_ { LOG_LEVEL };
				const char* filename_ { nullptr };
				uint32_t line_ { 0 };
				LogBufferStream logBufferStream_ {};
		} LogMessage;

		class Logger
		{
			public:
				Logger(LogLevel logLevel, const char* file, uint32_t line) noexcept;
				Logger(LogLevel logLevel, const char* file, uint32_t line, const char* fmt, ...) noexcept;
				~Logger();

			public:
				inline LogBufferStream& getMessageStream()
				{
					return logMessage_.getLogBufferStream();
				}

			private:
				LogMessage logMessage_;
		};
	} // namespace base
} // namespace nets

#define LOG_TRACE(fmt, ...)																	\
	do                         																\
	{                          																\
        if (LogLevel::TRACE >= LOG_LEVEL)													\
		{																					\
			nets::base::Logger(LogLevel::TRACE, __FILE__, __LINE__, fmt, ##__VA_ARGS__);	\
		}																					\
	}                          																\
	while (0)

#define LOG_DEBUG(fmt, ...) 																\
	do                         																\
	{                          																\
		if (LogLevel::DEBUG >= LOG_LEVEL) 													\
		{																					\
			nets::base::Logger(LogLevel::DEBUG, __FILE__, __LINE__, fmt, ##__VA_ARGS__);	\
		}																					\
	}                          																\
	while (0)

#define LOG_INFO(fmt, ...) 																\
	do                        															\
	{                          															\
		if (LogLevel::INFO >= LOG_LEVEL) 												\
		{																				\
			nets::base::Logger(LogLevel::INFO, __FILE__, __LINE__, fmt, ##__VA_ARGS__);	\
		}																				\
	}                          															\
	while (0)

#define LOG_WARN(fmt, ...) 															\
	do                         														\
	{                          														\
		nets::base::Logger(LogLevel::WARN, __FILE__, __LINE__, fmt, ##__VA_ARGS__);	\
	}                          														\
	while (0)

#define LOG_ERROR(fmt, ...) 															\
	do                         															\
	{                          															\
		nets::base::Logger(LogLevel::ERROR, __FILE__, __LINE__, fmt, ##__VA_ARGS__);	\
	}                          															\
	while (0)

#define LOG_FATAL(fmt, ...) 															\
	do                         															\
	{                         															\
		nets::base::Logger(LogLevel::FATAL, __FILE__, __LINE__, fmt, ##__VA_ARGS__);	\
	}                      																\
	while (0)


// stream api

#define LOGS_TRACE 																		\
		if (LogLevel::TRACE >= LOG_LEVEL) 												\
			nets::base::Logger(LogLevel::TRACE, __FILE__, __LINE__).getMessageStream()

#define LOGS_DEBUG 																		\
		if (LogLevel::DEBUG >= LOG_LEVEL) 												\
			nets::base::Logger(LogLevel::DEBUG, __FILE__, __LINE__).getMessageStream()

#define LOGS_INFO 																		\
		if (LogLevel::INFO >= LOG_LEVEL) 												\
			nets::base::Logger(LogLevel::INFO, __FILE__, __LINE__).getMessageStream()

#define LOGS_WARN 																		\
		if (LogLevel::WARN >= LOG_LEVEL) 												\
			nets::base::Logger(LogLevel::WARN, __FILE__, __LINE__).getMessageStream()

#define LOGS_ERROR 																		\
			nets::base::Logger(LogLevel::ERROR, __FILE__, __LINE__).getMessageStream()

#define LOGS_FATAL 																		\
			nets::base::Logger(LogLevel::FATAL, __FILE__, __LINE__).getMessageStream()

#endif //NETS_LOGGING_H
