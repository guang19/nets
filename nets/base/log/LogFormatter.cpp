//
// Created by guang19
//

#include "nets/base/log/LogFormatter.h"

#include "nets/base/CommonMacro.h"
#include "nets/base/exception/DateTimeFormatException.h"
#include "nets/base/ThreadHelper.h"

namespace nets
{
    INIT_SINGLETON(DefaultLogFormatter);

    namespace
    {
        const char* const gLogLevelName[LogLevel::NUM_OF_LOG_LEVELS] = {"TRACE", "DEBUG", "INFO", "WARN", "ERROR", "FATAL"};

        /**
         * log time cache
         */
        __thread ILogFormatter::Tm gCacheTms {};
        __thread Timestamp::TimeType gCacheSeconds {0};
    } // namespace

    void DefaultLogFormatter::formatLogTime(const Timestamp& logTime, LogBufferStream& logBufferStream)
    {
        Tm tms {};
        Timestamp::TimeType seconds = logTime.secondsSinceEpoch();
        if (seconds != gCacheSeconds)
        {
            if (nullptr == localtime_r(&seconds, &tms))
            {
                MEMZERO(&tms, sizeof(tms));
            }
            gCacheSeconds = seconds;
            gCacheTms = tms;
        }
        else
        {
            tms = gCacheTms;
        }
        char timeBuf[27] = {0};
        // check return value to circumvent [-Werror=format-truncation]
        ::snprintf(timeBuf, sizeof(timeBuf), "%04d-%02d-%02d %02d:%02d:%02d.%06ld", tms.tm_year + 1900, tms.tm_mon + 1,
                   tms.tm_mday, tms.tm_hour, tms.tm_min, tms.tm_sec, logTime.microsPartOfTimestamp()) < 0
            ? THROW_FMT(DateTimeFormatException, "DefaultLogFormatter format log time")
            : UNUSED(0);
        logBufferStream << timeBuf;
    }

    void DefaultLogFormatter::formatLogMessage(LogMessage& logMessage, LogBufferStream& logBufferStream)
    {
        formatLogTime(logMessage.getTime(), logBufferStream);
        logBufferStream << " [" << currentTid() << "] ";
        logBufferStream << gLogLevelName[logMessage.getLevel()] << ' ';
        logBufferStream << logMessage.getFile() << ':' << logMessage.getLine() << " - ";
        logBufferStream << logMessage.getStream();
    }

    ::std::shared_ptr<ILogFormatter> LogFormatterFactory::getLogFormatter()
    {
        return DefaultLogFormatter::getInstance();
    }
} // namespace nets