// MIT License
//
// Copyright (c) 2022 guang19
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

// @brief Log appender

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

    enum class LogFileType
    {
        SINGLE_FILE,
        DAILY_FILE,
        ROLLING_FILE
    };

    class FileLogAppender : public LogAppender
    {
    private:
        using FilePtr = ::std::unique_ptr<LogFile>;
        // Log buffer cache 2M
        static constexpr SizeType kLogBufferPieceSize = 1024 * 1024 << 1;
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
        void setLogFileRollingSize(SizeType rollingSize) override;

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