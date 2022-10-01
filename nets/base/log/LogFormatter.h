//
// Created by guang19
//

#ifndef NETS_LOG_FORMATTER_H
#define NETS_LOG_FORMATTER_H

#include "nets/base/log/LogMessage.h"
#include "nets/base/Singleton.h"

namespace nets
{
    class ILogFormatter
    {
    public:
        using Tm = struct tm;

    protected:
        virtual ~ILogFormatter() = default;

    public:
        virtual void formatLogTime(const Timestamp& logTime, LogBufferStream& logBufferStream) = 0;
        virtual void formatLogMessage(LogMessage& logMessage, LogBufferStream& logBufferStream) = 0;
    };

    DECLARE_SINGLETON_CLASS(DefaultLogFormatter), public ILogFormatter
    {
        DEFINE_SINGLETON(DefaultLogFormatter);

    public:
        void formatLogTime(const Timestamp& logTime, LogBufferStream& logBufferStream) override;
        void formatLogMessage(LogMessage& logMessage, LogBufferStream& logBufferStream) override;
    };

    class LogFormatterFactory
    {
    public:
        static ::std::shared_ptr<ILogFormatter> getLogFormatter();
    };
} // namespace nets

#define LOG_FORMATTER (nets::LogFormatterFactory::getLogFormatter())

#endif // NETS_LOG_FORMATTER_H