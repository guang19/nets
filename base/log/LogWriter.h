//
// Created by YangGuang on 2022/4/17.
//

#ifndef NETS_LOGWRITER_H
#define NETS_LOGWRITER_H

#include <atomic>
#include <condition_variable>
#include <thread>
#include <vector>
#include <sys/stat.h>
#include "base/log/LogBuffer.h"
#include "base/Noncopyable.h"
#include "base/Singleton.h"

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

		class LogFile : Noncopyable
		{
				using FileInfo = struct stat;

			public:
				explicit LogFile(const char* file);
				~LogFile();

			public:
				void append(const char* data, uint32_t len);
				void flush();

				void renameByNowTime();

				inline uint32_t size() const
				{
					return bytes_;
				}

				inline ::std::time_t createTime() const
				{
					return createTime_;
				}

				void getFileInfo(uint64_t* fileSize, ::std::time_t* createTime, ::std::time_t* modifyTime);

			private:
				FILE* fp_ { NULL };
				::std::string dir_ {};
				::std::string filename_ {};
				uint32_t bytes_ { 0 };
				::std::time_t createTime_ { 0 };
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
			DEFINE_SINGLETON(StdoutLogWriter)

			public:
				void write(const char* data, uint32_t len) override;
		};

		class AsyncFileLogWriter : public ILogWriter
		{
				using AtomicBoolType 		= ::std::atomic<bool>;
				using MutexType             = ::std::mutex;
				using ConditionVarType 		= ::std::condition_variable;
				using LockGuardType         = ::std::lock_guard<MutexType>;
				using UniqueLockType	    = ::std::unique_lock<MutexType>;
				using BufferPtr				= ::std::unique_ptr<LogBuffer>;
				using BufferVectorType		= ::std::vector<BufferPtr>;
				using FilePtr				= ::std::unique_ptr<LogFile>;

			public:
				AsyncFileLogWriter();
				~AsyncFileLogWriter() override;

				void write(const char* data, uint32_t len) override;

				void start();
				void stop();

				////////////////////////////////////////////////////////////////////////////
			protected:
				virtual void persist(const char* data, uint32_t len) = 0;
				FilePtr logFile_ { nullptr };
				////////////////////////////////////////////////////////////////////////////

			private:
				void asyncWrite();

			private:
				AtomicBoolType running_ { false };
				BufferPtr cacheBuffer_ { nullptr };
				BufferPtr backupCacheBuffer_ { nullptr };
				BufferVectorType buffers_ {};
				::std::thread asyncThread_ {};
				MutexType mtx_ {};
				ConditionVarType cv_ {};
		};

		DECLARE_SINGLETON_CLASS(AsyncSingleFileLogWriter), public AsyncFileLogWriter
		{
			DEFINE_SINGLETON(AsyncSingleFileLogWriter)

			protected:
				void persist(const char* data, uint32_t len) override;
		};

		DECLARE_SINGLETON_CLASS(AsyncDailyFileLogWriter), public AsyncFileLogWriter
		{
			DEFINE_SINGLETON(AsyncDailyFileLogWriter)

			protected:
				void persist(const char* data, uint32_t len) override;
		};

		DECLARE_SINGLETON_CLASS(AsyncRollingFileLogWriter), public AsyncFileLogWriter
		{
			DEFINE_SINGLETON(AsyncRollingFileLogWriter)

			protected:
				void persist(const char* data, uint32_t len) override;
		};

		DECLARE_SINGLETON_CLASS(LogWriterFactory)
		{
			DEFINE_SINGLETON(LogWriterFactory)

			public:
				ILogWriter* getLogWriter() const;
		};
	} // namespace base
} // namespace nets

#define LOG_WRITER (nets::base::LogWriterFactory::getInstance()->getLogWriter())

#endif //NETS_LOGWRITER_H
