//
// Created by YangGuang on 2022/4/1.
//

#include "nets/base/log/Logging.h"

#include "nets/base/log/LogFormatter.h"
#include "nets/base/log/LogWriter.h"

namespace nets::base
{
    LogMessageStream::LogMessageStream(LogLevel logLevel, const char* file, uint32_t line)
        : logMessage_(logLevel, file, line)
    {
    }

    LogMessageStream::~LogMessageStream()
    {
        logMessage_.getStream() << '\n';
        LogBufferStream logBufferStream {};
        LOG_FORMATTER->formatLogMessage(logMessage_, logBufferStream);
        LOG_WRITER->write(logBufferStream.buffer().carray(), logBufferStream.buffer().len());
        if (logMessage_.getLogLevel() == LogLevel::FATAL)
        {
            // if exit directly, log buffer in memory probably will lost
            ::std::this_thread::sleep_for(::std::chrono::milliseconds(MillisecondsPerSecond));
            ::fprintf(stderr, "Error:log fatal, exit\n");
            ::exit(1);
        }
    }
} // namespace nets::base
