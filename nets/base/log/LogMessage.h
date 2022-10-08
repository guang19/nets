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
        explicit LogMessage(LogLevel logLevel, const char* file, ::uint32_t line);
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

        inline ::uint32_t getLine() const
        {
            return line_;
        }

        inline LogBufferStream& getStream()
        {
            return stream_;
        }

    private:
        Timestamp logTime_;
        LogLevel logLevel_;
        const char* filename_;
        ::uint32_t line_;
        LogBufferStream stream_;
    };
} // namespace nets

#endif // NETS_LOG_MESSAGE_H