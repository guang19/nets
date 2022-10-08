//
// Created by guang19
//

#ifndef NETS_LOGGING_H
#define NETS_LOGGING_H

#include "nets/base/log/LogMessage.h"

#ifndef LOG_LEVEL
#define LOG_LEVEL nets::LogLevel::DEBUG
#endif

// stream api
#define LOGS_TRACE                                                                                                          \
    if (nets::LogLevel::TRACE >= LOG_LEVEL)                                                                                 \
    nets::LogMessageStream(nets::LogLevel::TRACE, __FILE__, __LINE__).stream()

#define LOGS_DEBUG                                                                                                          \
    if (nets::LogLevel::DEBUG >= LOG_LEVEL)                                                                                 \
    nets::LogMessageStream(nets::LogLevel::DEBUG, __FILE__, __LINE__).stream()

#define LOGS_INFO                                                                                                           \
    if (nets::LogLevel::INFO >= LOG_LEVEL)                                                                                  \
    nets::LogMessageStream(nets::LogLevel::INFO, __FILE__, __LINE__).stream()

#define LOGS_WARN                                                                                                           \
    if (nets::LogLevel::WARN >= LOG_LEVEL)                                                                                  \
    nets::LogMessageStream(nets::LogLevel::WARN, __FILE__, __LINE__).stream()

#define LOGS_ERROR nets::LogMessageStream(nets::LogLevel::ERROR, __FILE__, __LINE__).stream()

#define LOGS_FATAL nets::LogMessageStream(nets::LogLevel::FATAL, __FILE__, __LINE__).stream()

#endif // NETS_LOGGING_H