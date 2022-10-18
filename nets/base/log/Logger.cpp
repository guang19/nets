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

#include "nets/base/log/Logger.h"

#include <unistd.h>

namespace nets
{
    namespace
    {
        constexpr char kRootLoggerName[] = "RootLogger";
        constexpr LogLevel kDefaultLogLevel = LogLevel::DEBUG;
    } // namespace

    LoggerPtr kRootLogger = LOGGER_MGR->getRootLogger();

    LogMessage::LogMessage(LogLevel level, const char* file, Int32Type line)
        : time_(Timestamp::now()), level_(level), file_(nullptr), line_(line), stream_()
    {
        file_ = ::strrchr(file, '/');
        if (file_ != nullptr)
        {
            file_ = file_ + 1;
        }
    }

    LogMessageStream::LogMessageStream(LoggerPtr& logger, LogLevel logLevel, const char* file, Int32Type line)
        : logger_(logger), logMessage_(logLevel, file, line)
    {
    }

    LogMessageStream::~LogMessageStream()
    {
        logger_->log(logMessage_.getLevel(), logMessage_);
    }

    Logger::Logger(const StringType& name) : Logger(name, kDefaultLogLevel, nullptr, nullptr) {}

    Logger::Logger(const StringType& name, LogLevel level, const LogFormatterPtr& logFormatter,
                   const LogAppenderPtr& logAppender)
        : name_(name), level_(level), logFormatter_(logFormatter), logAppender_(logAppender)
    {
    }

    void Logger::setLogFormatter(const LogFormatterPtr& logFormatter)
    {
        LockGuardType lock(mutex_);
        logFormatter_ = logFormatter;
    }

    void Logger::setLogAppender(const LogAppenderPtr& logAppender)
    {
        LockGuardType lock(mutex_);
        logAppender_ = logAppender;
    }

    void Logger::setLogFileRollingSize(SizeType rollingSize)
    {
        LockGuardType lock(mutex_);
        logAppender_->setLogFileRollingSize(rollingSize);
    }

    void Logger::log(LogLevel level, const LogMessage& logMessage)
    {
        // format
        LogBuffer logBuffer {};
        logFormatter_->formatLogMessage(logMessage, logBuffer);
        // append
        logAppender_->append(logBuffer);
        if (logMessage.getLevel() == LogLevel::FATAL)
        {
            logAppender_->flush();
            // exit with a delay of one second to allow time for the log in memory to persist
            ::sleep(1);
            ::fprintf(stderr, "log fatal,exit\n");
            ::exit(1);
        }
    }

    void Logger::trace(const LogMessage& logMessage)
    {
        log(LogLevel::TRACE, logMessage);
    }

    void Logger::debug(const LogMessage& logMessage)
    {
        log(LogLevel::DEBUG, logMessage);
    }

    void Logger::info(const LogMessage& logMessage)
    {
        log(LogLevel::INFO, logMessage);
    }

    void Logger::warn(const LogMessage& logMessage)
    {
        log(LogLevel::WARN, logMessage);
    }

    void Logger::error(const LogMessage& logMessage)
    {
        log(LogLevel::ERROR, logMessage);
    }

    void Logger::fatal(const LogMessage& logMessage)
    {
        log(LogLevel::FATAL, logMessage);
    }

    INIT_SINGLETON(LoggerManager);

    LoggerManager::LoggerManager() : loggers_(), mutex_() {}

    LoggerPtr LoggerManager::getLogger(const StringType& loggerName)
    {
        LockGuardType lock(mutex_);
        auto it = loggers_.find(loggerName);
        if (it != loggers_.end())
        {
            return it->second;
        }
        else
        {
            auto logger = ::std::make_shared<Logger>(loggerName);
            logger->setLogFormatter(DefaultLogFormatter::getInstance());
            logger->setLogAppender(StdoutLogAppender::getInstance());
            loggers_[loggerName] = logger;
            return logger;
        }
    }

    LoggerPtr LoggerManager::getRootLogger()
    {
        return getLogger(kRootLoggerName);
    }
} // namespace nets