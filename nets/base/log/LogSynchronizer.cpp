//
// Created by guang19
//

#include "nets/base/log/LogSynchronizer.h"

#include "nets/base/log/LogWriter.h"

namespace nets::base
{
    INIT_SINGLETON(StdoutLogSynchronizer);
    INIT_SINGLETON(SingleLogFileSynchronizer);
    INIT_SINGLETON(DailyLogFileSynchronizer);
    INIT_SINGLETON(RollingLogFileSynchronizer);

    namespace
    {
        // if you want to test DAILY_FILE LogWriter, you need to adjust this
        // constant for short intervals, not for the whole day
        constexpr ::time_t gSecondsPerDay = 60 * 60 * 24;
        // Set SecondsPerDay to 30, then you can watch if the log file is roll back after 30s
        // constexpr ::time_t gSecondsPerDay = 30;

        // if you want to test ROLLING_FILE LogWriter, you need to adjust this
        // constant  as small as possible
        constexpr ::size_t gLogFileRollingSize = LOG_FILE_ROLLING_SIZE * 1024 * 1024;
        // Set LogFileRollingSize to 200 Bytes, then you will see soon if the log file is roll back
        // constexpr ::size_t gLogFileRollingSize = 200;
    } // namespace

    void StdoutLogSynchronizer::synchronize(const char* data, SizeType length, TimeType persistTime)
    {
        UNUSED(persistTime);
        ::fwrite(data, 1, length, stdout);
    }

    void StdoutLogSynchronizer::flush()
    {
        ::fflush(stdout);
    }

    LogFileSynchronizer::LogFileSynchronizer(const char* logFile) : logFile_(::std::make_unique<FileType>(logFile)) {}

    SingleLogFileSynchronizer::SingleLogFileSynchronizer(const char* logFile) : LogFileSynchronizer(logFile) {}

    void SingleLogFileSynchronizer::synchronize(const char* data, SizeType length, TimeType persistTime)
    {
        UNUSED(persistTime);
        logFile_->write(data, length);
    }

    void SingleLogFileSynchronizer::flush()
    {
        logFile_->flush();
    }

    DailyLogFileSynchronizer::DailyLogFileSynchronizer(const char* logFile) : LogFileSynchronizer(logFile) {}

    void DailyLogFileSynchronizer::synchronize(const char* data, SizeType length, TimeType now)
    {
        if (now - logFile_->lastRollTime() >= gSecondsPerDay)
        {
            logFile_->renameByNowTime(now);
        }
        logFile_->write(data, length);
    }

    void DailyLogFileSynchronizer::flush()
    {
        logFile_->flush();
    }

    RollingLogFileSynchronizer::RollingLogFileSynchronizer(const char* logFile) : LogFileSynchronizer(logFile) {}

    void RollingLogFileSynchronizer::synchronize(const char* data, SizeType length, TimeType now)
    {
        if (logFile_->size() + length > gLogFileRollingSize)
        {
            logFile_->renameByNowTime(now);
        }
        logFile_->write(data, length);
    }

    void RollingLogFileSynchronizer::flush()
    {
        logFile_->flush();
    }

    ::std::shared_ptr<LogSynchronizer> LogSynchronizerFactory::getLogSynchronizer()
    {
        switch (LOG_WRITER_TYPE)
        {
            case LogWriterType::STDOUT:
                return StdoutLogSynchronizer::getInstance();
            case LogWriterType::SINGLE_FILE:
                return SingleLogFileSynchronizer::getInstance(LOG_FILE);
            case LogWriterType::DAILY_FILE:
                return DailyLogFileSynchronizer::getInstance(LOG_FILE);
            case LogWriterType::ROLLING_FILE:
                return RollingLogFileSynchronizer::getInstance(LOG_FILE);
            default:
                return StdoutLogSynchronizer::getInstance();
        }
    }
} // namespace nets::base