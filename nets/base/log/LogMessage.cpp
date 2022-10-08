//
// Created by guang19
//

#include "nets/base/log/LogMessage.h"

#include <cstring>

#include "nets/base/log/LogFormatter.h"
#include "nets/base/log/LogWriter.h"

namespace nets
{
    LogMessage::LogMessage(LogLevel level, const char* file, ::int32_t line)
        : time_(Timestamp::now()), level_(level), file_(nullptr), line_(line), stream_()
    {
        file_ = ::strrchr(file, '/');
        if (file_ != nullptr)
        {
            file_ = file_ + 1;
        }
    }

    LogMessageStream::LogMessageStream(LogLevel logLevel, const char* file, ::uint32_t line)
        : logMessage_(logLevel, file, line)
    {
    }

    LogMessageStream::~LogMessageStream()
    {
        logMessage_.getStream() << '\n';
        LogBufferStream logBufferStream {};
        LOG_FORMATTER->formatLogMessage(logMessage_, logBufferStream);
        LOG_WRITER->write(logBufferStream.buffer().array(), logBufferStream.buffer().length());
        if (logMessage_.getLevel() == LogLevel::FATAL)
        {
            // if exit directly, log buffer in memory probably will lost
            ::std::this_thread::sleep_for(::std::chrono::milliseconds(gMillisecondsPerSecond));
            ::fprintf(stderr, "log fatal,exit\n");
            ::exit(1);
        }
    }
} // namespace nets