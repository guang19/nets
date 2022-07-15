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
#include "nets/base/log/LogFile.h"
#include "nets/base/Singleton.h"
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
        Stdout = 0,
        SingFile,
        DailyFile,
        RollingFile
    };

    namespace
    {
        // Log buffer cache 2M
        constexpr ::size_t LogBufferPieceSize = 1024 * 1024 << 1;
    } // namespace

    class Synchronizer
    {
    protected:
        using SizeType = ::size_t;
        using TimeType = ::time_t;

        virtual ~Synchronizer() = default;

    public:
        virtual void synchronize(const char* data, SizeType len, TimeType persistTime) = 0;
        virtual void flush() = 0;
    };

    DECLARE_SINGLETON_CLASS(StdoutSynchronizer), public Synchronizer
    {
        DEFINE_SINGLETON(StdoutSynchronizer);

    private:
        StdoutSynchronizer() = default;
        ~StdoutSynchronizer() override = default;

    public:
        void synchronize(const char* data, SizeType len, TimeType persistTime) override;
        void flush() override;
    };

    class LogFileSynchronizer : public Synchronizer
    {
    protected:
        using FileType = LogFile;
        using FilePtr = ::std::unique_ptr<FileType>;

    protected:
        explicit LogFileSynchronizer(const char* logFile);

    public:
        void synchronize(const char* data, SizeType len, TimeType persistTime) override = 0;
        void flush() override = 0;

    protected:
        FilePtr logFile_ {nullptr};
    };

    DECLARE_SINGLETON_CLASS(SingleLogFileSynchronizer), public LogFileSynchronizer
    {
        DEFINE_SINGLETON(SingleLogFileSynchronizer);

    private:
        explicit SingleLogFileSynchronizer(const char* logFile);
        ~SingleLogFileSynchronizer() override = default;

    public:
        void synchronize(const char* data, SizeType len, TimeType persistTime) override;
        void flush() override;
    };

    DECLARE_SINGLETON_CLASS(DailyLogFileSynchronizer), public LogFileSynchronizer
    {
        DEFINE_SINGLETON(DailyLogFileSynchronizer);

    private:
        explicit DailyLogFileSynchronizer(const char* logFile);
        ~DailyLogFileSynchronizer() override = default;

    public:
        void synchronize(const char* data, SizeType len, TimeType persistTime) override;
        void flush() override;
    };

    DECLARE_SINGLETON_CLASS(RollingLogFileSynchronizer), public LogFileSynchronizer
    {
        DEFINE_SINGLETON(RollingLogFileSynchronizer);

    private:
        explicit RollingLogFileSynchronizer(const char* logFile);
        ~RollingLogFileSynchronizer() override = default;

    public:
        void synchronize(const char* data, SizeType len, TimeType persistTime) override;
        void flush() override;
    };

    class SynchronizerFactory
    {
    public:
        static ::std::shared_ptr<Synchronizer> getSynchronizer();
    };

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
        using SynchronizerPtr = ::std::shared_ptr<Synchronizer>;
        using BufferVectorType = ::std::vector<BufferPtr>;
        using BufferVectorPtr = ::std::unique_ptr<BufferVectorType>;
        using SynchronizeTaskType = ::std::function<void()>;
        using BlockingQueueType = BoundedBlockingQueue<SynchronizeTaskType>;
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
        SynchronizerPtr synchronizer_ {nullptr};
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