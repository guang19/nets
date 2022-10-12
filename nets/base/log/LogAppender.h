//
// Created by guang19
//

#ifndef NETS_LOG_APPENDER_H
#define NETS_LOG_APPENDER_H

#include <atomic>
#include <condition_variable>
#include <memory>
#include <thread>
#include <vector>

#include "nets/base/log/LogBuffer.h"
#include "nets/base/log/LogFile.h"
#include "nets/base/Singleton.h"

namespace nets
{
    class LogAppender
    {
    protected:
        LogAppender() = default;
        virtual ~LogAppender() = default;

    public:
        virtual void append(const LogBuffer& logBuffer) = 0;
        virtual void flush() = 0;

        virtual void setLogFileRollingSize(SizeType rollingSize) {}
    };

    DECLARE_SINGLETON_CLASS(StdoutLogAppender), public LogAppender
    {
        DEFINE_SINGLETON(StdoutLogAppender);

    private:
        StdoutLogAppender() = default;
        ~StdoutLogAppender() override = default;

    public:
        void append(const LogBuffer& logBuffer) override;
        void flush() override;
    };

    enum LogFileType
    {
        SINGLE_FILE,
        DAILY_FILE,
        ROLLING_FILE
    };

    class FileLogAppender : public LogAppender
    {
    private:
        using SizeType = ::size_t;
        using TimeType = ::time_t;
        using FilePtr = ::std::unique_ptr<LogFile>;
        // Log buffer cache 2M
        static constexpr ::size_t kLogBufferPieceSize = 1024 * 1024 << 1;
        using BufferType = StackBuffer<kLogBufferPieceSize>;
        using BufferPtr = ::std::unique_ptr<BufferType>;
        using BufferVectorType = ::std::vector<BufferPtr>;
        using MutexType = ::std::mutex;
        using LockGuardType = ::std::lock_guard<MutexType>;
        using UniqueLockType = ::std::unique_lock<MutexType>;
        using ConditionVarType = ::std::condition_variable;
        using FileLogAppenderPtr = ::std::shared_ptr<FileLogAppender>;

    public:
        explicit FileLogAppender(const char* logFile, LogFileType logFileType);
        ~FileLogAppender() override;

        static FileLogAppenderPtr createFileLogAppender(const char* logFile, LogFileType logFileType);

    public:
        void append(const LogBuffer& logBuffer) override;
        void flush() override;

        // if you want to test ROLLING_FILE, you need to adjust logFileRollingSize as small as possible
        void setLogFileRollingSize(SizeType rollingSize) override
        {
            logFileRollingSize_ = rollingSize;
        }

    private:
        void sync();
        void syncFile(const BufferVectorType& buffers);
        void syncSingleFile(const BufferVectorType& buffers);
        void syncDailyFile(const BufferVectorType& buffers);
        void syncRollingFile(const BufferVectorType& buffers);

    private:
        ::std::atomic_bool running_;
        FilePtr logFile_;
        // unit:kb
        SizeType logFileRollingSize_;
        const LogFileType logFileType_;
        BufferPtr cacheBuffer_;
        BufferPtr backupCacheBuffer_;
        BufferVectorType buffers_;
        MutexType mutex_;
        ConditionVarType cv_;
        ::std::thread syncTask_;

        static constexpr SizeType kDefaultLogFileRollingSize = 1024;
    };
} // namespace nets

#endif // NETS_LOG_APPENDER_H