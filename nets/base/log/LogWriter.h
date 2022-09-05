//
// Created by guang19
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
        virtual void write(const char* data, SizeType length) = 0;
    };

    DECLARE_SINGLETON_CLASS(AsyncLogWriter), public LogWriter
    {
        DEFINE_SINGLETON(AsyncLogWriter);

    private:
        using BufferType = StackBuffer<LogBufferPieceSize>;
        using BufferPtr = ::std::unique_ptr<BufferType>;
        using BufferVectorType = ::std::vector<BufferPtr>;
        using LogSynchronizerPtr = ::std::shared_ptr<LogSynchronizer>;
        using MutexType = ::std::mutex;
        using LockGuardType = ::std::lock_guard<MutexType>;
        using UniqueLockType = ::std::unique_lock<MutexType>;
        using ConditionVarType = ::std::condition_variable;

    protected:
        AsyncLogWriter();
        ~AsyncLogWriter() override;

    public:
        void write(const char* data, SizeType length) override;

    private:
        void afterInit()
        {
            start();
        }

        void start();
        void swap();

    private:
        ::std::atomic_bool running_ {false};
        BufferPtr cacheBuffer_ {nullptr};
        BufferPtr backupCacheBuffer_ {nullptr};
        BufferVectorType buffers_ {};
        LogSynchronizerPtr logSynchronizer_ {nullptr};
        ::std::thread writerTask {};
        MutexType mutex_ {};
        ConditionVarType cv_ {};
    };
} // namespace nets::base

#define LOG_WRITER (nets::base::AsyncLogWriter::getInstance())

#endif // NETS_BASE_LOG_WRITER_H