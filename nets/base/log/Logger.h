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

// @brief Nets logger framework

#ifndef NETS_LOGGER_H
#define NETS_LOGGER_H

#include <map>

#include "nets/base/log/LogAppender.h"
#include "nets/base/log/LogFormatter.h"

#define NETS_LOG(LOGGER, LEVEL)                                                                                             \
    if (LEVEL >= LOGGER->getLevel())                                                                                        \
    nets::LogMessageStream(LOGGER, LEVEL, __FILE__, __LINE__).stream()

#define NETS_SYSTEM_LOG(LEVEL) NETS_LOG(nets::kRootLogger, LEVEL)

// log by root logger
#define NETS_SYSTEM_LOG_TRACE NETS_SYSTEM_LOG(nets::LogLevel::TRACE)

#define NETS_SYSTEM_LOG_DEBUG NETS_SYSTEM_LOG(nets::LogLevel::DEBUG)

#define NETS_SYSTEM_LOG_INFO NETS_SYSTEM_LOG(nets::LogLevel::INFO)

#define NETS_SYSTEM_LOG_WARN NETS_SYSTEM_LOG(nets::LogLevel::WARN)

#define NETS_SYSTEM_LOG_ERROR NETS_SYSTEM_LOG(nets::LogLevel::ERROR)

#define NETS_SYSTEM_LOG_FATAL NETS_SYSTEM_LOG(nets::LogLevel::FATAL)

// log by customized logger
#define NETS_LOG_TRACE(LOGGER) NETS_LOG(LOGGER, nets::LogLevel::TRACE)

#define NETS_LOG_DEBUG(LOGGER) NETS_LOG(LOGGER, nets::LogLevel::DEBUG)

#define NETS_LOG_INFO(LOGGER) NETS_LOG(LOGGER, nets::LogLevel::INFO)

#define NETS_LOG_WARN(LOGGER) NETS_LOG(LOGGER, nets::LogLevel::WARN)

#define NETS_LOG_ERROR(LOGGER) NETS_LOG(LOGGER, nets::LogLevel::ERROR)

#define NETS_LOG_FATAL(LOGGER) NETS_LOG(LOGGER, nets::LogLevel::FATAL)

#define LOGGER_MGR (nets::LoggerManager::getInstance())

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

    class Logger;
    using LoggerPtr = ::std::shared_ptr<Logger>;

    class LogMessage : Noncopyable
    {
    public:
        explicit LogMessage(LogLevel level, const char* file, Int32Type line);
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

        inline Int32Type getLine() const
        {
            return line_;
        }

        inline LogBufferStream& getStream()
        {
            return stream_;
        }

        inline const LogBufferStream& getStream() const
        {
            return stream_;
        }

    private:
        Timestamp time_;
        LogLevel level_;
        const char* file_;
        Int32Type line_;
        LogBufferStream stream_;
    };

    class LogMessageStream : Noncopyable
    {
    public:
        explicit LogMessageStream(LoggerPtr& logger, LogLevel logLevel, const char* file, Int32Type line);
        ~LogMessageStream();

    public:
        inline LogBufferStream& stream()
        {
            return logMessage_.getStream();
        }

    private:
        LoggerPtr logger_;
        LogMessage logMessage_;
    };

    class Logger : Noncopyable
    {
    public:
        using MutexType = ::std::mutex;
        using LockGuardType = ::std::lock_guard<MutexType>;
        using LogAppenderPtr = ::std::shared_ptr<LogAppender>;
        using LogFormatterPtr = ::std::shared_ptr<LogFormatter>;

    public:
        explicit Logger(const StringType& name);
        explicit Logger(const StringType& name, LogLevel level, const LogFormatterPtr& logFormatter,
                        const LogAppenderPtr& logAppender);
        ~Logger() = default;

    public:
        inline void setLevel(LogLevel level)
        {
            level_ = level;
        }

        inline LogLevel getLevel() const
        {
            return level_;
        }

        void setLogFormatter(const LogFormatterPtr& logFormatter);
        void setLogAppender(const LogAppenderPtr& logAppender);

        void setLogFileRollingSize(SizeType rollingSize);

    public:
        void log(LogLevel level, const LogMessage& logMessage);
        void trace(const LogMessage& logMessage);
        void debug(const LogMessage& logMessage);
        void info(const LogMessage& logMessage);
        void warn(const LogMessage& logMessage);
        void error(const LogMessage& logMessage);
        void fatal(const LogMessage& logMessage);

    private:
        StringType name_;
        LogLevel level_;
        LogFormatterPtr logFormatter_;
        LogAppenderPtr logAppender_;
        MutexType mutex_;
    };

    DECLARE_SINGLETON_CLASS(LoggerManager)
    {
        DEFINE_SINGLETON(LoggerManager);

    private:
        LoggerManager();
        ~LoggerManager() = default;

    private:
        using LoggerContainer = ::std::map<StringType, LoggerPtr>;
        using MutexType = ::std::mutex;
        using LockGuardType = ::std::lock_guard<MutexType>;

    public:
        LoggerPtr getLogger(const StringType& loggerName);
        LoggerPtr getRootLogger();

    private:
        LoggerContainer loggers_;
        MutexType mutex_;
    };

    extern LoggerPtr kRootLogger;
} // namespace nets

#endif // NETS_LOGGER_H