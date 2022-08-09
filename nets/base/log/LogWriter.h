//
// Created by YangGuang on 2022/4/17.
//

#ifndef NETS_BASE_LOG_WRITER_H
#define NETS_BASE_LOG_WRITER_H

#include <atomic>
#include <memory>
#include <thread>
#include <vector>

#include "nets/base/concurrency/BoundedBlockingQueue.h"
#include "nets/base/log/LogSynchronizer.h"
#include "nets/base/StackBuffer.h"

#ifndef LOG_WRITER_TYPE
#define LOG_WRITER_TYPE STDOUT
#endif

#ifndef LOG_FILE
#define LOG_FILE "/tmp/nets/nets.log"
#endif

#ifndef LOG_FILE_ROLLING_SIZE
#define LOG_FILE_ROLLING_SIZE 24
#endif

namespace nets::base
{
    enum LogWriterType
    {
        STDOUT = 0,
        SINGLE_FILE,
        DAILY_FILE,
        ROLLING_FILE
    };

    namespace
    {
        // Log buffer cache 2M
        constexpr ::size_t LogBufferPieceSize = 1024 * 1024 << 1;
    } // namespace

    class LogWriter
    {
    protected:
        using SizeType = ::size_t;
        using TimeType = ::time_t;

        LogWriter() = default;
        virtual ~LogWriter() = default;

    public:
        virtual void write(const char* data, SizeType len) = 0;
    };

    DECLARE_SINGLETON_CLASS(AsyncLogWriter), public LogWriter
    {
        DEFINE_SINGLETON(AsyncLogWriter);

    private:
        using BufferType = StackBuffer<LogBufferPieceSize>;
        using BufferPtr = ::std::unique_ptr<BufferType>;
        using MutexType = ::std::mutex;
        using LockGuardType = ::std::lock_guard<MutexType>;
        using UniqueLockType = ::std::unique_lock<MutexType>;
        using ConditionVarType = ::std::condition_variable;
        using LogSynchronizerPtr = ::std::shared_ptr<LogSynchronizer>;
        using BufferVectorType = ::std::vector<BufferPtr>;
        using BufferVectorPtr = ::std::unique_ptr<BufferVectorType>;
        using LogSynchronizeTaskType = ::std::function<void()>;
        using BlockingQueueType = BoundedBlockingQueue<LogSynchronizeTaskType>;
        using BlockingQueuePtr = ::std::unique_ptr<BlockingQueueType>;

    protected:
        AsyncLogWriter();
        ~AsyncLogWriter() override;

    public:
        void write(const char* data, SizeType len) override;

    private:
        void afterInit()
        {
            start();
        }

        void start();
        void swap();
        void synchronize();

    private:
        ::std::atomic_bool running_ {false};
        BufferPtr cacheBuffer_ {nullptr};
        BufferPtr backupCacheBuffer_ {nullptr};
        LogSynchronizerPtr logSynchronizer_ {nullptr};
        // add persistent tasks to the task queue
        ::std::thread writerTaskProducer {};
        // take the persistent task from the task queue and execute it
        ::std::thread writerTaskConsumer {};
        MutexType mutex_ {};
        ConditionVarType cv_ {};
        BufferVectorPtr buffers_ {};
        BlockingQueuePtr writerTaskQueue_ {nullptr};
    };
} // namespace nets::base

#define LOG_WRITER (nets::base::AsyncLogWriter::getInstance())

#endif // NETS_BASE_LOG_WRITER_H