//
// Created by YangGuang on 2022/4/17.
//

#include "nets/base/log/LogWriter.h"

#include <cassert>

namespace nets::base
{
    namespace
    {
        // if you want to test DAILY_FILE LogWriter, you need to adjust this
        // constant for short intervals, not for the whole day
        constexpr ::time_t SecondsPerDay = 60 * 60 * 24;
        // Set SecondsPerDay to 30, then you can watch if the log file is roll back after 30s
        // constexpr ::time_t SecondsPerDay = 30;

        // if you want to test ROLLING_FILE LogWriter, you need to adjust this
        // constant  as small as possible
        constexpr ::size_t LogFileRollingSize = LOG_FILE_ROLLING_SIZE * 1024 * 1024;
        // Set LogFileRollingSize to 200 Bytes, then you will see soon if the log file is roll back
        // constexpr ::size_t LogFileRollingSize = 200;

        // Log buffer flush interval,unit：milliseconds
        constexpr ::time_t LogBufferFlushInterval = 1000;
    } // namespace

    INIT_SINGLETON(StdoutSynchronizer);
    INIT_SINGLETON(SingleLogFileSynchronizer);
    INIT_SINGLETON(DailyLogFileSynchronizer);
    INIT_SINGLETON(RollingLogFileSynchronizer);
    INIT_SINGLETON(AsyncLogWriter);

    void StdoutSynchronizer::synchronize(const char* data, SizeType len, TimeType persistTime)
    {
        UNUSED(persistTime);
        ::fwrite(data, 1, len, stdout);
    }

    void StdoutSynchronizer::flush()
    {
        ::fflush(stdout);
    }

    LogFileSynchronizer::LogFileSynchronizer(const char* logFile) : logFile_(::std::make_unique<FileType>(logFile)) {}

    SingleLogFileSynchronizer::SingleLogFileSynchronizer(const char* logFile) : LogFileSynchronizer(logFile) {}

    void SingleLogFileSynchronizer::synchronize(const char* data, SizeType len, TimeType persistTime)
    {
        UNUSED(persistTime);
        logFile_->write(data, len);
    }

    void SingleLogFileSynchronizer::flush()
    {
        logFile_->flush();
    }

    DailyLogFileSynchronizer::DailyLogFileSynchronizer(const char* logFile) : LogFileSynchronizer(logFile) {}

    void DailyLogFileSynchronizer::synchronize(const char* data, SizeType len, TimeType persistTime)
    {
        if (persistTime - logFile_->lastRollTime() >= SecondsPerDay)
        {
            logFile_->renameByNowTime(persistTime);
        }
        logFile_->write(data, len);
    }

    void DailyLogFileSynchronizer::flush()
    {
        logFile_->flush();
    }

    RollingLogFileSynchronizer::RollingLogFileSynchronizer(const char* logFile) : LogFileSynchronizer(logFile) {}

    void RollingLogFileSynchronizer::synchronize(const char* data, SizeType len, TimeType persistTime)
    {
        if (logFile_->size() + len > LogFileRollingSize)
        {
            logFile_->renameByNowTime(persistTime);
        }
        logFile_->write(data, len);
    }

    void RollingLogFileSynchronizer::flush()
    {
        logFile_->flush();
    }

    ::std::shared_ptr<Synchronizer> SynchronizerFactory::getSynchronizer()
    {
        switch (LOG_WRITER_TYPE)
        {
            case LogWriterType::Stdout:
                return StdoutSynchronizer::getInstance();
            case LogWriterType::SingFile:
                return SingleLogFileSynchronizer::getInstance(LOG_FILE);
            case LogWriterType::DailyFile:
                return DailyLogFileSynchronizer::getInstance(LOG_FILE);
            case LogWriterType::RollingFile:
                return RollingLogFileSynchronizer::getInstance(LOG_FILE);
            default:
                return StdoutSynchronizer::getInstance();
        }
    }

    AsyncLogWriter::AsyncLogWriter()
        : running_(false), cacheBuffer_(::std::make_unique<BufferType>()),
          backupCacheBuffer_(::std::make_unique<BufferType>()), synchronizer_(SynchronizerFactory::getSynchronizer()),
          mutex_(), cv_(), buffers_(::std::make_unique<BufferVectorType>()),
          writerTaskQueue_(::std::make_unique<BlockingQueueType>())
    {
        buffers_->reserve(LOG_FILE_ROLLING_SIZE >> 1);
    }

    AsyncLogWriter::~AsyncLogWriter()
    {
        if (running_)
        {
            running_ = false;
            cv_.notify_one();
            if (writerTaskProducer.joinable())
            {
                writerTaskProducer.join();
            }
            if (writerTaskConsumer.joinable())
            {
                writerTaskConsumer.join();
            }
        }
    }

    void AsyncLogWriter::start()
    {
        if (running_)
        {
            return;
        }
        running_ = true;
        writerTaskProducer = ::std::thread(&AsyncLogWriter::swap, this);
        writerTaskConsumer = ::std::thread(&AsyncLogWriter::synchronize, this);
    }

    void AsyncLogWriter::write(const char* data, SizeType len)
    {
        LockGuardType lock(mutex_);
        if (cacheBuffer_->writableBytes() > len)
        {
            cacheBuffer_->writeBytes(data, len);
        }
        else
        {
            buffers_->push_back(::std::move(cacheBuffer_));
            if (backupCacheBuffer_ != nullptr)
            {
                cacheBuffer_ = ::std::move(backupCacheBuffer_);
            }
            else
            {
                cacheBuffer_ = std::make_unique<BufferType>();
            }
            cacheBuffer_->writeBytes(data, len);
            cv_.notify_one();
        }
    }

    void AsyncLogWriter::swap()
    {
        auto buffers = ::std::make_unique<BufferVectorType>();
        buffers->reserve(LOG_FILE_ROLLING_SIZE >> 1);
        auto tmpBuffer1 = ::std::make_unique<BufferType>();
        auto tmpBuffer2 = ::std::make_unique<BufferType>();
        TimeType currentTime = 0;
        while (running_)
        {
            {
                UniqueLockType lock(mutex_);
                // first determine whether the buffer vector is empty, and then determine whether cache buffer has data
                cv_.wait_for(lock, ::std::chrono::milliseconds(LogBufferFlushInterval),
                             [this]() -> bool
                             {
                                 return !buffers_->empty();
                             });
                if (buffers_->empty() && cacheBuffer_->len() <= 0)
                {
                    lock.unlock();
                    continue;
                }
                buffers_->push_back(::std::move(cacheBuffer_));
                buffers->swap(*buffers_);
                cacheBuffer_ = ::std::move(tmpBuffer1);
                if (nullptr == backupCacheBuffer_)
                {
                    backupCacheBuffer_ = ::std::move(tmpBuffer2);
                }
            }
            ::time(&currentTime);
            auto synchronizeTask = [this, tmpBuffers = buffers.release(), currentTime]() mutable
            {
                for (auto& it: *tmpBuffers)
                {
                    auto logBuffer = it.get();
                    synchronizer_->synchronize(logBuffer->array(), logBuffer->len(), currentTime);
                }
                synchronizer_->flush();
            };
            // if not running, the writer task that is not added to the queue will be discarded
            writerTaskQueue_->put(::std::move(synchronizeTask));
            buffers = ::std::make_unique<BufferVectorType>();
            tmpBuffer1 = std::make_unique<BufferType>();
            if (nullptr == tmpBuffer2)
            {
                tmpBuffer2 = std::make_unique<BufferType>();
            }
        }
        // last check
        assert(!running_);
        if (!buffers_->empty() || (cacheBuffer_->len() > 0))
        {
            buffers_->push_back(::std::move(cacheBuffer_));
            ::time(&currentTime);
            auto writerTask = [this, tmpBuffers = buffers_.release(), currentTime]() mutable
            {
                for (auto& it: *tmpBuffers)
                {
                    auto logBuffer = it.get();
                    synchronizer_->synchronize(logBuffer->array(), logBuffer->len(), currentTime);
                }
                synchronizer_->flush();
            };
            writerTaskQueue_->put(::std::move(writerTask));
        }
    }

    void AsyncLogWriter::synchronize()
    {
        SynchronizeTaskType synchronizeTask = nullptr;
        while (running_)
        {
            if (writerTaskQueue_->take(synchronizeTask, LogBufferFlushInterval >> 1))
            {
                synchronizeTask();
                synchronizeTask = nullptr;
            }
        }
        // last check
        assert(!running_);
        if (writerTaskQueue_->take(synchronizeTask, LogBufferFlushInterval))
        {
            synchronizeTask();
            synchronizeTask = nullptr;
        }
    }
} // namespace nets::base