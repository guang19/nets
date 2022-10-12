//
// Created by guang19
//

#include "nets/base/log/LogAppender.h"

namespace nets
{
    namespace
    {
        constexpr ::int32_t kMaximumOfLogBuffer = 24;

        // Log buffer flush interval,unit：milliseconds
        constexpr ::time_t kLogBufferFlushInterval = 1000;

        // if you want to test DAILY_FILE, you need to adjust kSecondsPerDay for short intervals, not for the whole day
        constexpr ::time_t kSecondsPerDay = 60 * 60 * 24;
        // Set SecondsPerDay to 30, then you can watch if the log file is roll back after 30s
        // constexpr ::time_t kSecondsPerDay = 30;
    } // namespace

    INIT_SINGLETON(StdoutLogAppender);

    void StdoutLogAppender::append(const LogBuffer& logBuffer)
    {
        ::fwrite(logBuffer.array(), sizeof(char), logBuffer.length(), stdout);
        // if in Debug mode, flush log buffer
#ifndef NDEBUG
        flush();
#endif
    }

    void StdoutLogAppender::flush()
    {
        ::fflush(stdout);
    }

    FileLogAppender::FileLogAppender(const char* logFile, LogFileType logFileType)
        : running_(true), logFile_(::std::make_unique<LogFile>(logFile)), logFileRollingSize_(kDefaultLogFileRollingSize),
          logFileType_(logFileType), cacheBuffer_(::std::make_unique<BufferType>()),
          backupCacheBuffer_(::std::make_unique<BufferType>()), buffers_(), mutex_(), cv_(),
          syncTask_(&FileLogAppender::sync, this)
    {
    }

    FileLogAppender::~FileLogAppender()
    {
        if (running_)
        {
            running_ = false;
            cv_.notify_one();
            if (syncTask_.joinable())
            {
                syncTask_.join();
            }
        }
    }

    FileLogAppender::FileLogAppenderPtr FileLogAppender::createFileLogAppender(const char* logFile, LogFileType logFileType)
    {
        return ::std::make_shared<FileLogAppender>(logFile, logFileType);
    }

    void FileLogAppender::append(const LogBuffer& logBuffer)
    {
        LockGuardType lock(mutex_);
        const char* data = logBuffer.array();
        SizeType length = logBuffer.length();
        if (cacheBuffer_->writableBytes() > length)
        {
            cacheBuffer_->writeBytes(data, length);
        }
        else
        {
            buffers_.push_back(::std::move(cacheBuffer_));
            if (backupCacheBuffer_ != nullptr)
            {
                cacheBuffer_ = ::std::move(backupCacheBuffer_);
            }
            else
            {
                cacheBuffer_ = std::make_unique<BufferType>();
            }
            cacheBuffer_->writeBytes(data, length);
            cv_.notify_one();
        }
    }

    void FileLogAppender::flush()
    {
        logFile_->flush();
    }

    void FileLogAppender::sync()
    {
        BufferVectorType buffers {};
        buffers.reserve(buffers_.size());
        auto cacheBuffer = ::std::make_unique<BufferType>();
        auto backupCacheBuffer = ::std::make_unique<BufferType>();
        while (running_)
        {
            {
                UniqueLockType lock(mutex_);
                // first determine whether the buffer vector is empty, and then determine whether cache buffer has data
                cv_.wait_for(lock, ::std::chrono::milliseconds(kLogBufferFlushInterval),
                             [this]() -> bool
                             {
                                 return !buffers_.empty() || cacheBuffer_->length() > 0;
                             });
                if (!buffers_.empty() || cacheBuffer_->length() > 0)
                {
                    buffers_.push_back(::std::move(cacheBuffer_));
                    buffers.swap(buffers_);
                    cacheBuffer_ = ::std::move(cacheBuffer);
                    if (backupCacheBuffer_ != nullptr)
                    {
                        backupCacheBuffer_ = ::std::move(backupCacheBuffer);
                    }
                }
                else
                {
                    lock.unlock();
                    continue;
                }
            }
            if (buffers.size() > kMaximumOfLogBuffer)
            {
                SizeType bufferSize = buffers.size();
                buffers.erase(buffers.begin() + static_cast<::int64_t>(bufferSize >> 1), buffers.end());
                char warning[255] = {0};
                ::snprintf(warning, sizeof(warning), "Dropped %ld log buffers\n", bufferSize >> 1);
                auto buf = std::make_unique<BufferType>();
                buf->writeBytes(warning);
                buffers.push_back(::std::move(buf));
            }
            syncFile(buffers);
            buffers.clear();
            cacheBuffer = std::make_unique<BufferType>();
            if (backupCacheBuffer == nullptr)
            {
                backupCacheBuffer = std::make_unique<BufferType>();
            }
            logFile_->flush();
        }
        logFile_->flush();
    }

    void FileLogAppender::syncFile(const BufferVectorType& buffers)
    {
        switch (logFileType_)
        {
            case SINGLE_FILE:
            {
                syncSingleFile(buffers);
                break;
            }
            case DAILY_FILE:
            {
                syncDailyFile(buffers);
                break;
            }
            case ROLLING_FILE:
            {
                syncRollingFile(buffers);
                break;
            }
        }
    }

    void FileLogAppender::syncSingleFile(const BufferVectorType& buffers)
    {
        for (const auto& buffer : buffers)
        {
            logFile_->write(buffer->array(), buffer->length());
        }
    }

    void FileLogAppender::syncDailyFile(const BufferVectorType& buffers)
    {
        TimeType now = 0;
        ::time(&now);
        for (const auto& buffer : buffers)
        {
            if (kSecondsPerDay < now - logFile_->lastRollTime())
            {
                logFile_->renameByNowTime(now);
            }
            logFile_->write(buffer->array(), buffer->length());
        }
    }

    void FileLogAppender::syncRollingFile(const BufferVectorType& buffers)
    {
        TimeType now = 0;
        ::time(&now);
        for (const auto& buffer : buffers)
        {
            if ((logFile_->size() + buffer->length()) / 1024 >= logFileRollingSize_)
            {
                logFile_->renameByNowTime(now);
            }
            logFile_->write(buffer->array(), buffer->length());
        }
    }
} // namespace nets