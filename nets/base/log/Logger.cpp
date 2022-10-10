//
// Created by guang19
//

#include "nets/base/log/Logger.h"

namespace nets
{
    namespace
    {
        constexpr char kRootLoggerName[] = "RootLogger";
        constexpr LogLevel kDefaultLogLevel = LogLevel::DEBUG;
    } // namespace

    LoggerPtr kRootLogger = LOGGER_MGR->getRootLogger();

    LogMessage::LogMessage(LogLevel level, const char* file, ::int32_t line)
        : time_(Timestamp::now()), level_(level), file_(nullptr), line_(line), stream_()
    {
        file_ = ::strrchr(file, '/');
        if (file_ != nullptr)
        {
            file_ = file_ + 1;
        }
    }

    LogMessageStream::LogMessageStream(LoggerPtr& logger, LogLevel logLevel, const char* file, ::uint32_t line)
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
        : name_(name), logFormatter_(logFormatter), logAppender_(logAppender)
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

    void Logger::log(LogLevel level, const LogMessage& logMessage)
    {
        // format
        LogBuffer logBuffer {};
        logFormatter_->formatLogMessage(logMessage, logBuffer);
        // append
        logAppender_->append(logBuffer);
        //        LOG_WRITER->write(logBufferStream.buffer().array(), logBufferStream.buffer().length());
        //        if (logMessage.getLevel() == LogLevel::FATAL)
        //        {
        // if exit directly, log buffer in memory probably will lost
        //            ::std::this_thread::sleep_for(::std::chrono::milliseconds(kMillisecondsPerSecond));
        //            ::fprintf(stderr, "log fatal,exit\n");
        //            ::exit(1);
        //        }
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