// MIT License
//
// Copyright (c) 2022 guang19
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

// @brief Log formatter

#include "nets/base/log/LogFormatter.h"

#include "nets/base/Common.h"
#include "nets/base/exception/DateTimeFormatException.h"
#include "nets/base/log/Logger.h"
#include "nets/base/ThreadHelper.h"

namespace nets
{
    INIT_SINGLETON(DefaultLogFormatter);

    namespace
    {
        const char* const kLogLevelName[ENUM_CLASS_TO_INT(LogLevel::NUM_OF_LOG_LEVELS)] = {"TRACE", "DEBUG", "INFO",
                                                                                           "WARN",  "ERROR", "FATAL"};

        constexpr Int32Type kMaxLengthOfLogTimePortion = 52;
        constexpr char kLF = '\n';
        constexpr char kSpace = ' ';
        constexpr char kColon = ':';
        constexpr char kOpenSquareBracket[] = " [";
        constexpr char kCloseSquareBracket[] = "] ";
        constexpr char kDash[] = " - ";

        /**
         * log time cache
         */
        using Tm = struct tm;
        __thread Tm tCacheTms {};
        __thread TimeType tCacheSeconds {0};
    } // namespace

    void DefaultLogFormatter::formatLogMessage(const LogMessage& logMessage, LogBuffer& logBuffer)
    {
        formatLogTime(logMessage.getTime(), logBuffer);
        logBuffer.writeBytes(kOpenSquareBracket, 2);
        logBuffer.writeInt32(currentTid());
        logBuffer.writeBytes(kCloseSquareBracket, 2);
        logBuffer.writeBytes(kLogLevelName[ENUM_CLASS_TO_INT(logMessage.getLevel())]);
        logBuffer.writeByte(kSpace);
        logBuffer.writeBytes(logMessage.getFile());
        logBuffer.writeByte(kColon);
        logBuffer.writeInt32(logMessage.getLine());
        logBuffer.writeBytes(kDash, 3);
        logBuffer.writeBytes(logMessage.getStream().buffer());
        logBuffer.writeByte(kLF);
    }

    void DefaultLogFormatter::formatLogTime(const Timestamp& logTime, LogBuffer& logBuffer)
    {
        Tm tms {};
        TimeType seconds = logTime.secondsSinceEpoch();
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
            ? THROW_FMT(DateTimeFormatException, "DefaultLogFormatter format log time error")
            : UNUSED(0);
        logBuffer.writeBytes(timeBuf);
    }
} // namespace nets