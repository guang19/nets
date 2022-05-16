//
// Created by YangGuang on 2022/4/1.
//

#ifndef NETS_LOGGING_H
#define NETS_LOGGING_H

#include "nets/base/log/LogMessage.h"

#ifndef LOG_LEVEL
#define LOG_LEVEL 1
#endif

namespace nets::base
{
	class LogMessageStream : public LogBuffer
	{
	public:
		explicit LogMessageStream(LogLevel logLevel, const char* file, uint32_t line);
		~LogMessageStream() override;

	public:
		inline LogBuffer& stream()
		{
			return logMessage_.getStream();
		}

	private:
		LogMessage logMessage_;
	};
} // namespace nets::base

// stream api
#define LOGS_TRACE                                                                                                          \
	if (nets::base::LogLevel::TRACE >= LOG_LEVEL)                                                                                       \
	nets::base::LogMessageStream(nets::base::LogLevel::TRACE, __FILE__, __LINE__).stream()

#define LOGS_DEBUG                                                                                                          \
	if (nets::base::LogLevel::DEBUG >= LOG_LEVEL)                                                                                       \
	nets::base::LogMessageStream(nets::base::LogLevel::DEBUG, __FILE__, __LINE__).stream()

#define LOGS_INFO                                                                                                           \
	if (nets::base::LogLevel::INFO >= LOG_LEVEL)                                                                                        \
	nets::base::LogMessageStream(nets::base::LogLevel::INFO, __FILE__, __LINE__).stream()

#define LOGS_WARN                                                                                                           \
	if (nets::base::LogLevel::WARN >= LOG_LEVEL)                                                                                        \
	nets::base::LogMessageStream(nets::base::LogLevel::WARN, __FILE__, __LINE__).stream()

#define LOGS_ERROR nets::base::LogMessageStream(nets::base::LogLevel::ERROR, __FILE__, __LINE__).stream()

#define LOGS_FATAL nets::base::LogMessageStream(nets::base::LogLevel::FATAL, __FILE__, __LINE__).stream()

#endif // NETS_LOGGING_H
