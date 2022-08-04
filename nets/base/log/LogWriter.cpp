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
        constexpr ::time_t LogBufferFlushInterval = 1000;
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
        }
    }
} // namespace nets::base