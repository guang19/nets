//
// Created by YangGuang on 2022/4/17.
//

#ifndef NETS_LOGWRITER_H
#define NETS_LOGWRITER_H

#include <atomic>
#include <memory>
#include <vector>
#include "nets/base/concurrency/ConditionVariable.h"
#include "nets/base/concurrency/Mutex.h"
#include "nets/base/log/LogBuffer.h"
#include "nets/base/Noncopyable.h"
#include "nets/base/Singleton.h"
#include "nets/base/Thread.h"

#ifndef LOG_WRITER_TYPE
#define LOG_WRITER_TYPE STDOUT
#endif

#ifndef LOG_FILE
#define LOG_FILE "/tmp/nets/nets.log"
#endif

#ifndef LOG_FILE_ROLLING_SIZE
#define LOG_FILE_ROLLING_SIZE 24
#endif

namespace nets
{
	namespace base
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
				explicit LogFile(const char* file);
				~LogFile();

			public:
				void append(const char* data, uint32_t len);
				void flush();

				void renameByNowTime(::time_t now);

				void mkdirR(const char* multiLevelDir);

				inline uint32_t size() const
				{
					return bytes_;
				}

				inline ::time_t lastRollTime() const
				{
					return lastRollTime_;
				}

			private:
				void getFileInfo(uint64_t* fileSize, ::time_t* createTime);

			private:
				FILE* fp_ { nullptr };
				char* dir_ { nullptr };
				char* filename_ { nullptr };
				uint64_t bytes_ { 0 };
				::time_t lastRollTime_ { 0 };
				char* buffer_ { nullptr };
		};

		class ILogWriter
		{
			protected:
				virtual ~ILogWriter() = default;

			public:
				virtual void write(const char* data, uint32_t len) = 0;
		};

		DECLARE_SINGLETON_CLASS(StdoutLogWriter), public ILogWriter
		{
			DEFINE_SINGLETON(StdoutLogWriter);

			public:
				void write(const char* data, uint32_t len) override;
		};

		class AsyncFileLogWriter : public ILogWriter
		{
				using FilePtr = ::std::unique_ptr<LogFile>;
				using BufferPtr = ::std::unique_ptr<LogBuffer>;
				using MutexType = Mutex;
				using LockGuardType = LockGuard<MutexType>;
				using AtomicBoolType = ::std::atomic<bool>;
				using ConditionVarType = ConditionVariable;
				using BufferVectorType = ::std::vector<BufferPtr>;

			public:
				AsyncFileLogWriter();
				~AsyncFileLogWriter() override;

				void write(const char* data, uint32_t len) override;

				void start();
				void stop();

				////////////////////////////////////////////////////////////////////////////
			protected:
				virtual void persist(const char* data, uint32_t len, ::time_t persistTime){};
				FilePtr logFile_ { nullptr };
				////////////////////////////////////////////////////////////////////////////

			private:
				void asyncWrite();

			private:
				AtomicBoolType running_ { false };
				BufferPtr cacheBuffer_ { nullptr };
				BufferPtr backupCacheBuffer_ { nullptr };
				BufferVectorType buffers_ {};
				Thread asyncThread_ {};
				MutexType mutex_ {};
				ConditionVarType cv_ {};
		};

		DECLARE_SINGLETON_CLASS(AsyncSingleFileLogWriter), public AsyncFileLogWriter
		{
			DEFINE_SINGLETON(AsyncSingleFileLogWriter);

			private:
				inline void afterInit()
				{
					start();
				}

			protected:
				void persist(const char* data, uint32_t len, ::time_t persistTime) override;
		};

		DECLARE_SINGLETON_CLASS(AsyncDailyFileLogWriter), public AsyncFileLogWriter
		{
			DEFINE_SINGLETON(AsyncDailyFileLogWriter);

			private:
				inline void afterInit()
				{
					start();
				}

			protected:
				void persist(const char* data, uint32_t len, ::time_t persistTime) override;
		};

		DECLARE_SINGLETON_CLASS(AsyncRollingFileLogWriter), public AsyncFileLogWriter
		{
			DEFINE_SINGLETON(AsyncRollingFileLogWriter);

			private:
				inline void afterInit()
				{
					start();
				}

			protected:
				void persist(const char* data, uint32_t len, ::time_t persistTime) override;
		};

		DECLARE_SINGLETON_CLASS(LogWriterFactory)
		{
			DEFINE_SINGLETON(LogWriterFactory);

			public:
				ILogWriter* getLogWriter() const;
		};
	} // namespace base
} // namespace nets

#define LOG_WRITER (nets::base::LogWriterFactory::getInstance()->getLogWriter())

#endif //NETS_LOGWRITER_H
