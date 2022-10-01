//
// Created by guang19
//

#ifndef NETS_LOGGER_H
#define NETS_LOGGER_H

#include <string>

#include "nets/base/Noncopyable.h"

namespace nets
{
    class LogLevel
    {
    public:
        enum Level
        {
            TRACE = 0,
            DEBUG,
            INFO,
            WARN,
            ERROR,
            FATAL,
            NUM_OF_LOG_LEVELS
        };

        const char* const gLogLevelName[LogLevel::NUM_OF_LOG_LEVELS] = {"TRACE", "DEBUG", "INFO", "WARN", "ERROR", "FATAL"};
    };

    class Logger : Noncopyable
    {
    public:
        using StringType = ::std::string;

    public:
        explicit Logger(const StringType& name);
        ~Logger() = default;

    private:
        ::std::string name_;
    };
} // namespace nets

#endif // NETS_LOGGER_H