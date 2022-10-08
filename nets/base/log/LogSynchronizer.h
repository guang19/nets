//
// Created by guang19
//

#ifndef NETS_LOG_SYNCHRONIZER_H
#define NETS_LOG_SYNCHRONIZER_H

#include "nets/base/log/LogFile.h"
#include "nets/base/Singleton.h"

namespace nets
{
    class LogSynchronizer
    {
    protected:
        using SizeType = ::size_t;
        using TimeType = ::time_t;

        virtual ~LogSynchronizer() = default;

    public:
        virtual void synchronize(const char* data, SizeType length, TimeType persistTime) = 0;
        virtual void flush() = 0;
    };

    DECLARE_SINGLETON_CLASS(StdoutLogSynchronizer), public LogSynchronizer
    {
        DEFINE_SINGLETON(StdoutLogSynchronizer);

    private:
        StdoutLogSynchronizer() = default;
        ~StdoutLogSynchronizer() override = default;

    public:
        void synchronize(const char* data, SizeType length, TimeType persistTime) override;
        void flush() override;
    };

    class LogFileSynchronizer : public LogSynchronizer
    {
    protected:
        using FileType = LogFile;
        using FilePtr = ::std::unique_ptr<FileType>;

    protected:
        explicit LogFileSynchronizer(const char* logFile);

    public:
        void synchronize(const char* data, SizeType length, TimeType now) override = 0;
        void flush() override = 0;

    protected:
        FilePtr logFile_;
    };

    DECLARE_SINGLETON_CLASS(SingleLogFileSynchronizer), public LogFileSynchronizer
    {
        DEFINE_SINGLETON(SingleLogFileSynchronizer);

    private:
        explicit SingleLogFileSynchronizer(const char* logFile);
        ~SingleLogFileSynchronizer() override = default;

    public:
        void synchronize(const char* data, SizeType length, TimeType now) override;
        void flush() override;
    };

    DECLARE_SINGLETON_CLASS(DailyLogFileSynchronizer), public LogFileSynchronizer
    {
        DEFINE_SINGLETON(DailyLogFileSynchronizer);

    private:
        explicit DailyLogFileSynchronizer(const char* logFile);
        ~DailyLogFileSynchronizer() override = default;

    public:
        void synchronize(const char* data, SizeType length, TimeType persistTime) override;
        void flush() override;
    };

    DECLARE_SINGLETON_CLASS(RollingLogFileSynchronizer), public LogFileSynchronizer
    {
        DEFINE_SINGLETON(RollingLogFileSynchronizer);

    private:
        explicit RollingLogFileSynchronizer(const char* logFile);
        ~RollingLogFileSynchronizer() override = default;

    public:
        void synchronize(const char* data, SizeType length, TimeType persistTime) override;
        void flush() override;
    };

    class LogSynchronizerFactory
    {
    public:
        static ::std::shared_ptr<LogSynchronizer> getLogSynchronizer();
    };
} // namespace nets

#endif // NETS_LOG_SYNCHRONIZER_H