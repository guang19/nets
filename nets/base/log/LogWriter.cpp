//
// Created by YangGuang on 2022/4/17.
//

#include "nets/base/log/LogWriter.h"

#include <cassert>

namespace nets::base
{
    INIT_SINGLETON(AsyncLogWriter);

    namespace
    {
        // Log buffer flush interval,unit：milliseconds
        constexpr ::time_t LogBufferFlushInterval = 500;
    }

    AsyncLogWriter::AsyncLogWriter()
        : running_(false), cacheBuffer_(::std::make_unique<BufferType>()),
          backupCacheBuffer_(::std::make_unique<BufferType>()), logSynchronizer_(LogSynchronizerFactory::getLogSynchronizer()),
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

    void AsyncLogWriter::write(const char* data, SizeType length)
    {
        LockGuardType lock(mutex_);
        if (cacheBuffer_->writableBytes() > length)
        {
            cacheBuffer_->writeBytes(data, length);
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
            cacheBuffer_->writeBytes(data, length);
            cv_.notify_one();
        }
    }

    void AsyncLogWriter::swap()
    {
        auto tmpBuffers = ::std::make_unique<BufferVectorType>();
        tmpBuffers->reserve(buffers_->size());
        auto tmpBuffer1 = ::std::make_unique<BufferType>();
        auto tmpBuffer2 = ::std::make_unique<BufferType>();
        while (running_)
        {
            {
                UniqueLockType lock(mutex_);
                // first determine whether the buffer vector is empty, and then determine whether cache buffer has data
                cv_.wait_for(lock, ::std::chrono::milliseconds(LogBufferFlushInterval),
                             [this]() -> bool
                             {
                                 return !buffers_->empty() || cacheBuffer_->length() > 0;
                             });
                if (buffers_->empty() && cacheBuffer_->length() == 0)
                {
                    lock.unlock();
                    continue;
                }
                buffers_->push_back(::std::move(cacheBuffer_));
                tmpBuffers->swap(*buffers_);
                cacheBuffer_ = ::std::move(tmpBuffer1);
                if (nullptr == backupCacheBuffer_)
                {
                    backupCacheBuffer_ = ::std::move(tmpBuffer2);
                }
            }
            TimeType currentTime = 0;
            ::time(&currentTime);
            auto logSynchronizeTask = [this, tmpBuffers = tmpBuffers.release(), currentTime]() mutable
            {
                for (auto& it: *tmpBuffers)
                {
                    auto logBuffer = it.get();
                    logSynchronizer_->synchronize(logBuffer->array(), logBuffer->length(), currentTime);
                }
                logSynchronizer_->flush();
            };
            // if not running, the writer task that is not added to the queue will be discarded
            writerTaskQueue_->put(::std::move(logSynchronizeTask));
            tmpBuffers = ::std::make_unique<BufferVectorType>();
            tmpBuffer1 = std::make_unique<BufferType>();
            if (nullptr == tmpBuffer2)
            {
                tmpBuffer2 = std::make_unique<BufferType>();
            }
        }
        // last check
        assert(!running_);
        if (!buffers_->empty() || cacheBuffer_->length() > 0)
        {
            buffers_->push_back(::std::move(cacheBuffer_));
            TimeType currentTime = 0;
            ::time(&currentTime);
            auto writerTask = [this, tmpBuffers = buffers_.release(), currentTime]() mutable
            {
                for (auto& it: *tmpBuffers)
                {
                    auto logBuffer = it.get();
                    logSynchronizer_->synchronize(logBuffer->array(), logBuffer->length(), currentTime);
                }
                logSynchronizer_->flush();
            };
            writerTaskQueue_->put(::std::move(writerTask));
        }
    }

    void AsyncLogWriter::synchronize()
    {
        LogSynchronizeTaskType logSynchronizeTask = nullptr;
        while (running_)
        {
            if (writerTaskQueue_->take(logSynchronizeTask, LogBufferFlushInterval >> 1))
            {
                logSynchronizeTask();
                logSynchronizeTask = nullptr;
            }
        }
        // last check
        assert(!running_);
        if (writerTaskQueue_->take(logSynchronizeTask, LogBufferFlushInterval >> 1))
        {
            logSynchronizeTask();
        }
    }
} // namespace nets::base