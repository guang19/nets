//
// Created by guang19
//

#ifndef NETS_LOG_FORMATTER_H
#define NETS_LOG_FORMATTER_H

#include "nets/base/log/LogBuffer.h"
#include "nets/base/Singleton.h"
#include "nets/base/Timestamp.h"

namespace nets
{
    class LogMessage;
    class LogFormatter
    {
    protected:
        LogFormatter() = default;
        virtual ~LogFormatter() = default;

    public:
        virtual void formatLogTime(const Timestamp& logTime, LogBuffer& logBuffer) = 0;
        virtual void formatLogMessage(const LogMessage& logMessage, LogBuffer& logBuffer) = 0;
    };

    DECLARE_SINGLETON_CLASS(DefaultLogFormatter), public LogFormatter
    {
        DEFINE_SINGLETON(DefaultLogFormatter);

    private:
        DefaultLogFormatter() = default;
        ~DefaultLogFormatter() override = default;

    public:
        void formatLogTime(const Timestamp& logTime, LogBuffer& logBuffer) override;
        void formatLogMessage(const LogMessage& logMessage, LogBuffer& logBuffer) override;
    };
} // namespace nets

#endif // NETS_LOG_FORMATTER_H