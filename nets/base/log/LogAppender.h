//
// Created by guang19
//

#ifndef NETS_LOG_APPENDER_H
#define NETS_LOG_APPENDER_H

#include "nets/base/log/LogBuffer.h"
#include "nets/base/Singleton.h"

namespace nets
{
    class LogAppender
    {
    protected:
        LogAppender() = default;
        virtual ~LogAppender() = default;

    public:
        virtual void append(const LogBuffer& logBuffer) = 0;
    };

    DECLARE_SINGLETON_CLASS(StdoutLogAppender), public LogAppender
    {
        DEFINE_SINGLETON(StdoutLogAppender);

    private:
        StdoutLogAppender() = default;
        ~StdoutLogAppender() override = default;

    public:
        void append(const LogBuffer& logBuffer) override;
    };
} // namespace nets

#endif // NETS_LOG_APPENDER_H