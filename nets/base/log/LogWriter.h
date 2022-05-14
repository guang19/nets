//
// Created by YangGuang on 2022/4/17.
//

#ifndef NETS_LOGWRITER_H
#define NETS_LOGWRITER_H

#include <atomic>
#include <condition_variable>
#include <cstdint>
#include <ctime>
#include <functional>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

#include "nets/base/log/LogBuffer.h"
#include "nets/base/Noncopyable.h"
#include "nets/base/Singleton.h"

#include "nets/base/concurrency/BoundedBlockingQueue.h"

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

	/**
	 *  not thread-safe
	 */
	class LogFile : Noncopyable
	{
	public:
		using SizeType = ::size_t;
		using TimeType = ::time_t;
		using CharPtr = ::std::unique_ptr<char[]>;

	public:
		explicit LogFile(const char* file);
		~LogFile();

	public:
		void append(const char* data, SizeType len);
		void flush();

		void renameByNowTime(TimeType now);

		void mkdirR(const char* multiLevelDir);

		inline SizeType size() const
		{
			return bytes_;
		}

		inline TimeType lastRollTime() const
		{
			return lastRollTime_;
		}

	private:
		void getFileInfo(uint64_t* fileSize, ::time_t* createTime);

	private:
		FILE* fp_ {nullptr};
		CharPtr dir_ {nullptr};
		CharPtr file_ {nullptr};
		SizeType bytes_ {0};
		TimeType lastRollTime_ {0};
		CharPtr buffer_ {nullptr};
	};

	class IPersistentWriter
	{
	protected:
		using SizeType = ::size_t;
		using TimeType = ::time_t;
		using FileType = LogFile;
		using FilePtr = ::std::unique_ptr<FileType>;

		virtual ~IPersistentWriter() = default;

	public:
		virtual void persist(const char* data, SizeType len, TimeType persistTime) = 0;
		virtual void flush() = 0;
	};

	DECLARE_SINGLETON_CLASS(StdoutPersistentWriter), public IPersistentWriter
	{
		DEFINE_SINGLETON(StdoutPersistentWriter);

	private:
		StdoutPersistentWriter() = default;
		~StdoutPersistentWriter() override = default;

	public:
		void persist(const char* data, SizeType len, TimeType persistTime) override;
		void flush() override;
	};

	DECLARE_SINGLETON_CLASS(SingleLogFilePersistentWriter), public IPersistentWriter
	{
		DEFINE_SINGLETON(SingleLogFilePersistentWriter);

	private:
		SingleLogFilePersistentWriter();
		~SingleLogFilePersistentWriter() override = default;

	public:
		void persist(const char* data, SizeType len, TimeType persistTime) override;
		void flush() override;

	private:
		FilePtr logFile_ {nullptr};
	};

	DECLARE_SINGLETON_CLASS(DailyLogFilePersistentWriter), public IPersistentWriter
	{
		DEFINE_SINGLETON(DailyLogFilePersistentWriter);

	private:
		DailyLogFilePersistentWriter();
		~DailyLogFilePersistentWriter() override = default;

	public:
		void persist(const char* data, SizeType len, TimeType persistTime) override;
		void flush() override;

	private:
		FilePtr logFile_ {nullptr};
	};

	DECLARE_SINGLETON_CLASS(RollingLogFilePersistentWriter), public IPersistentWriter
	{
		DEFINE_SINGLETON(RollingLogFilePersistentWriter);

	private:
		RollingLogFilePersistentWriter();
		~RollingLogFilePersistentWriter() override = default;

	public:
		void persist(const char* data, SizeType len, TimeType persistTime) override;
		void flush() override;

	private:
		FilePtr logFile_ {nullptr};
	};

	class PersistentWriterFactory
	{
	public:
		static ::std::shared_ptr<IPersistentWriter> getPersistentWriter();
	};

	class ILogWriter
	{
	protected:
		using SizeType = ::size_t;
		using TimeType = ::time_t;

		ILogWriter() = default;
		virtual ~ILogWriter() = default;

	public:
		virtual void write(const char* data, SizeType len) = 0;
	};

	DECLARE_SINGLETON_CLASS(AsyncLogWriter), public ILogWriter
	{
		DEFINE_SINGLETON(AsyncLogWriter);

	private:
		using BufferType = LogBuffer;
		using BufferPtr = ::std::unique_ptr<BufferType>;
		using MutexType = ::std::mutex;
		using LockGuardType = ::std::lock_guard<MutexType>;
		using UniqueLockType = ::std::unique_lock<MutexType>;
		using AtomicBoolType = ::std::atomic<bool>;
		using ConditionVarType = ::std::condition_variable;
		using PersistentWriterPtr = ::std::shared_ptr<IPersistentWriter>;
		using BufferVectorType = ::std::vector<BufferPtr>;
		using BufferVectorPtr = ::std::unique_ptr<BufferVectorType>;
		using WriterTaskType = ::std::function<void()>;
		using BlockingQueueType = BoundedBlockingQueue<WriterTaskType>;
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
		void asyncWrite();
		void asyncPersist();

	private:
		AtomicBoolType running_ {false};
		BufferPtr cacheBuffer_ {nullptr};
		BufferPtr backupCacheBuffer_ {nullptr};
		PersistentWriterPtr persistentWriter_ {nullptr};
		::std::thread writerTaskProducer {};
		::std::thread writerTaskConsumer {};
		MutexType mutex_ {};
		ConditionVarType cv_ {};
		BufferVectorPtr buffers_ {};
		BlockingQueuePtr writerTaskQueue_ {nullptr};
	};
} // namespace nets::base

#define LOG_WRITER (nets::base::AsyncLogWriter::getInstance())

#endif // NETS_LOGWRITER_H