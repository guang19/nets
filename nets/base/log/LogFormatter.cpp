//
// Created by guang19
//

#include "nets/base/log/LogFormatter.h"

#include "nets/base/CommonMacro.h"
#include "nets/base/exception/DateTimeFormatException.h"
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
        __thread ILogFormatter::Tm CacheTMS {};
        __thread Timestamp::TimeType CacheSeconds {0};
    } // namespace

    void DefaultLogFormatter::formatLogTime(const Timestamp& logTime, LogBufferStream& logBufferStream)
    {
        Tm tmS {};
        Timestamp::TimeType seconds = logTime.seconds();
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
        char timeBuf[27] = {0};
        // check return value to circumvent [-Werror=format-truncation]
        ::snprintf(timeBuf, sizeof(timeBuf), "%04d-%02d-%02d %02d:%02d:%02d.%06ld", tmS.tm_year + 1900, tmS.tm_mon + 1,
                   tmS.tm_mday, tmS.tm_hour, tmS.tm_min, tmS.tm_sec, logTime.microsPartOfTimestamp()) < 0
            ? THROW_FMT(DateTimeFormatException, "DefaultLogFormatter format log time")
            : UNUSED(0);
        logBufferStream << timeBuf;
    }

    void DefaultLogFormatter::formatLogMessage(LogMessage& logMessage, LogBufferStream& logBufferStream)
    {
        formatLogTime(logMessage.getLogTime(), logBufferStream);
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