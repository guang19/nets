//
// Created by YangGuang on 2022/5/7.
//

#ifndef NETS_BASE_LOG_MESSAGE_H
#define NETS_BASE_LOG_MESSAGE_H

#include "nets/base/log/LogBufferStream.h"
#include "nets/base/Timestamp.h"

namespace nets::base
{
    enum LogLevel
    {
        TRACE = 0,
        DEBUG,
        INFO,
        WARN,
        ERROR,
        FATAL,
        NUM_OF_LOG_LEVELS
    };

    class LogMessage : Noncopyable
    {
    public:
        explicit LogMessage(LogLevel logLevel, const char* file, uint32_t line);
        ~LogMessage() = default;

    public:
        inline const Timestamp& getLogTime() const
        {
            return logTime_;
        }

        inline LogLevel getLogLevel() const
        {
            return logLevel_;
        }

        inline const char* getFilename() const
        {
            return filename_;
        }

        inline uint32_t getLine() const
        {
            return line_;
        }

        inline LogBufferStream& getStream()
        {
            return stream_;
        }

    private:
        Timestamp logTime_ {};
        LogLevel logLevel_ {};
        const char* filename_ {nullptr};
        uint32_t line_ {0};
        LogBufferStream stream_ {};
    };
} // namespace nets::base

#endif // NETS_BASE_LOG_MESSAGE_H