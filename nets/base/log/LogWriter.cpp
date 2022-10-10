//
// Created by guang19
//

#include "nets/base/log/LogWriter.h"

#include <cassert>

namespace nets
{
    INIT_SINGLETON(AsyncLogWriter);

    namespace
    {
        constexpr ::int32_t kMaximumOfLogBuffer = 40;
        // Log buffer flush interval,unit：milliseconds
        constexpr ::time_t kLogBufferFlushInterval = 1000;
    } // namespace

    AsyncLogWriter::AsyncLogWriter()
        : running_(false), cacheBuffer_(::std::make_unique<BufferType>()),
          backupCacheBuffer_(::std::make_unique<BufferType>()), buffers_(),
          logSynchronizer_(LogSynchronizerFactory::getLogSynchronizer()), mutex_(), cv_()
    {
        buffers_.reserve(LOG_FILE_ROLLING_SIZE >> 1);
    }

    AsyncLogWriter::~AsyncLogWriter()
    {
        if (running_)
        {
            running_ = false;
            cv_.notify_one();
            if (writerTask_.joinable())
            {
                writerTask_.join();
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
        writerTask_ = ::std::thread(&AsyncLogWriter::swap, this);
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

    void AsyncLogWriter::swap()
    {
        BufferVectorType tmpBuffers {};
        tmpBuffers.reserve(buffers_.size());
        auto tmpCacheBuffer = ::std::make_unique<BufferType>();
        auto tmpBackupCacheBuffer = ::std::make_unique<BufferType>();
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
                buffers_.push_back(::std::move(cacheBuffer_));
                tmpBuffers.swap(buffers_);
                cacheBuffer_ = ::std::move(tmpCacheBuffer);
                if (backupCacheBuffer_ != nullptr)
                {
                    backupCacheBuffer_ = ::std::move(tmpBackupCacheBuffer);
                }
            }
            TimeType currentTime = 0;
            ::time(&currentTime);
            if (tmpBuffers.size() > kMaximumOfLogBuffer)
            {
                SizeType size = tmpBuffers.size();
                char warning[255] = {0};
                ::snprintf(warning, sizeof(warning), "Dropped %ldM logs\n",
                           (size >> 1) * (kLogBufferPieceSize / 1024 / 1024));
                logSynchronizer_->synchronize(warning, ::strlen(warning), currentTime);
                tmpBuffers.erase(tmpBuffers.begin() + static_cast<::int64_t>((size >> 1)), tmpBuffers.end());
            }
            for (const auto& buffer: tmpBuffers)
            {
                logSynchronizer_->synchronize(buffer->array(), buffer->length(), currentTime);
            }
            logSynchronizer_->flush();
            // if not running, the writer task that is not added to the queue will be discarded
            tmpBuffers.clear();
            tmpCacheBuffer = std::make_unique<BufferType>();
            if (tmpBackupCacheBuffer == nullptr)
            {
                tmpBackupCacheBuffer = std::make_unique<BufferType>();
            }
        }
        logSynchronizer_->flush();
    }
} // namespace nets