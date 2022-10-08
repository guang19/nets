//
// Created by guang19
//

#ifndef NETS_LOG_MESSAGE_H
#define NETS_LOG_MESSAGE_H

#include "nets/base/log/LogBufferStream.h"
#include "nets/base/Timestamp.h"

namespace nets
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
        explicit LogMessage(LogLevel level, const char* file, ::int32_t line);
        ~LogMessage() = default;

    public:
        inline const Timestamp& getTime() const
        {
            return time_;
        }

        inline LogLevel getLevel() const
        {
            return level_;
        }

        inline const char* getFile() const
        {
            return file_;
        }

        inline ::int32_t getLine() const
        {
            return line_;
        }

        inline LogBufferStream& getStream()
        {
            return stream_;
        }

    private:
        Timestamp time_;
        LogLevel level_;
        const char* file_;
        ::int32_t line_;
        LogBufferStream stream_;
    };


    class LogMessageStream : Noncopyable
    {
    public:
        explicit LogMessageStream(LogLevel logLevel, const char* file, ::uint32_t line);
        ~LogMessageStream();

    public:
        inline LogBufferStream& stream()
        {
            return logMessage_.getStream();
        }

    private:
        LogMessage logMessage_;
    };
} // namespace nets

#endif // NETS_LOG_MESSAGE_H