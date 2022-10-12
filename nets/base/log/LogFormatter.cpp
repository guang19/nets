//
// Created by guang19
//

#include "nets/base/log/LogFormatter.h"

#include "nets/base/CommonMacro.h"
#include "nets/base/exception/DateTimeFormatException.h"
#include "nets/base/log/Logger.h"
#include "nets/base/ThreadHelper.h"

namespace nets
{
    INIT_SINGLETON(DefaultLogFormatter);

    namespace
    {
        const char* const kLogLevelName[LogLevel::NUM_OF_LOG_LEVELS] = {"TRACE", "DEBUG", "INFO", "WARN", "ERROR", "FATAL"};

        constexpr ::int32_t kMaxLengthOfLogTimePortion = 52;
        constexpr char kLF = '\n';
        constexpr char kSpace = ' ';
        constexpr char kColon = ':';
        constexpr char kOpenSquareBracket[] = {kSpace, '['};
        constexpr char kCloseSquareBracket[] = {']', kSpace};
        constexpr char kDash[] = {kSpace, '-', kSpace};

        /**
         * log time cache
         */
        using Tm = struct tm;
        __thread Tm tCacheTms {};
        __thread Timestamp::TimeType tCacheSeconds {0};
    } // namespace

    void DefaultLogFormatter::formatLogMessage(const LogMessage& logMessage, LogBuffer& logBuffer)
    {
        formatLogTime(logMessage.getTime(), logBuffer);
        logBuffer.writeBytes(kOpenSquareBracket, sizeof(kOpenSquareBracket));
        logBuffer.writeInt32(currentTid());
        logBuffer.writeBytes(kCloseSquareBracket, sizeof(kOpenSquareBracket));
        logBuffer.writeBytes(kLogLevelName[logMessage.getLevel()]);
        logBuffer.writeByte(kSpace);
        logBuffer.writeBytes(logMessage.getFile());
        logBuffer.writeByte(kColon);
        logBuffer.writeInt32(logMessage.getLine());
        logBuffer.writeBytes(kDash, sizeof(kDash));
        logBuffer.writeBytes(logMessage.getStream().buffer());
        logBuffer.writeByte(kLF);
    }

    void DefaultLogFormatter::formatLogTime(const Timestamp& logTime, LogBuffer& logBuffer)
    {
        Tm tms {};
        Timestamp::TimeType seconds = logTime.secondsSinceEpoch();
        if (seconds != tCacheSeconds)
        {
            if (nullptr == localtime_r(&seconds, &tms))
            {
                MEMZERO(&tms, sizeof(tms));
            }
            tCacheSeconds = seconds;
            tCacheTms = tms;
        }
        else
        {
            tms = tCacheTms;
        }
        char timeBuf[kMaxLengthOfLogTimePortion] = {0};
        // check return value to circumvent [-Werror=format-truncation]
        ::snprintf(timeBuf, sizeof(timeBuf), "%04d-%02d-%02d %02d:%02d:%02d.%06ld", tms.tm_year + 1900, tms.tm_mon + 1,
                   tms.tm_mday, tms.tm_hour, tms.tm_min, tms.tm_sec, logTime.microsPartOfTimestamp()) <= 0
            ? THROW_FMT(DateTimeFormatException, "DefaultLogFormatter format log time")
            : UNUSED(0);
        logBuffer.writeBytes(timeBuf);
    }
} // namespace nets