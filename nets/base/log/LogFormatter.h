//
// Created by guang19
//

#ifndef NETS_LOG_FORMATTER_H
#define NETS_LOG_FORMATTER_H

#include "nets/base/Singleton.h"
#include "nets/base/Timestamp.h"
#include "nets/base/log/LogBufferStream.h"

namespace nets
{
    class LogMessage;
    class LogFormatter
    {
    public:
        using Tm = struct tm;

    protected:
        virtual ~LogFormatter() = default;

    public:
        virtual void formatLogTime(const Timestamp& logTime, LogBufferStream& logBufferStream) = 0;
        virtual void formatLogMessage(const LogMessage& logMessage, LogBufferStream& logBufferStream) = 0;
    };

    DECLARE_SINGLETON_CLASS(DefaultLogFormatter), public LogFormatter
    {
        DEFINE_SINGLETON(DefaultLogFormatter);

    public:
        void formatLogTime(const Timestamp& logTime, LogBufferStream& logBufferStream) override;
        void formatLogMessage(const LogMessage& logMessage, LogBufferStream& logBufferStream) override;
    };

    class LogFormatterFactory
    {
    public:
        static ::std::shared_ptr<LogFormatter> getLogFormatter();
    };
} // namespace nets

#define LOG_FORMATTER (nets::LogFormatterFactory::getLogFormatter())

#endif // NETS_LOG_FORMATTER_H