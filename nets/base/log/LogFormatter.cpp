//
// Created by YangGuang on 2022/4/21.
//

#include "nets/base/log/LogFormatter.h"

#include "nets/base/CommonMacro.h"
#include "nets/base/ThreadHelper.h"

namespace nets::base
{
    INIT_SINGLETON(DefaultLogFormatter);

    namespace
    {
        const char* const LogLevelName[LogLevel::NUM_OF_LOG_LEVELS] = {"TRACE", "DEBUG", "INFO", "WARN", "ERROR", "FATAL"};

        /**
         * log time cache
         */
        __thread struct tm CacheTMS {};
        __thread Timestamp::TimeType CacheSeconds {0};
    } // namespace

    void DefaultLogFormatter::formatLogMessage(LogMessage& logMessage, LogBufferStream& logBufferStream)
    {
        struct tm tmS {};
        const Timestamp& logTime = logMessage.getLogTime();
        Timestamp::TimeType seconds = logTime.secsFromTimestamp();
        if (seconds != CacheSeconds)
        {
            if (nullptr == localtime_r(&seconds, &tmS))
            {
                MEMZERO(&tmS, sizeof(tmS));
            }
            CacheSeconds = seconds;
            CacheTMS = tmS;
        }
        else
        {
            tmS = CacheTMS;
        }
        char timeBuf[32] = {0};
        // check return value to circumvent [-Werror=format-truncation]
        ::snprintf(timeBuf, sizeof(timeBuf), "%04d-%02d-%02d %02d:%02d:%02d.%03d", tmS.tm_year + 1900, tmS.tm_mon + 1,
                   tmS.tm_mday, tmS.tm_hour, tmS.tm_min, tmS.tm_sec, logTime.microsFromTimestamp()) < 0
            ? throw ::std::runtime_error("DefaultLogFormatter formatLogMessage snprintf format truncation exception")
            : UNUSED(0);
        logBufferStream << timeBuf;
        logBufferStream << " [" << currentTid() << "] ";
        logBufferStream << LogLevelName[logMessage.getLogLevel()] << ' ';
        logBufferStream << logMessage.getFilename() << ':' << logMessage.getLine() << " - ";
        logBufferStream << logMessage.getStream();
    }

    ::std::shared_ptr<ILogFormatter> LogFormatterFactory::getLogFormatter()
    {
        return DefaultLogFormatter::getInstance();
    }
} // namespace nets::base