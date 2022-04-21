//
// Created by YangGuang on 2022/4/17.
//

#ifndef NETS_LOGWRITER_H
#define NETS_LOGWRITER_H

#include <list>
#include <thread>
#include "base/Singleton.h"
#include "base/log/LogBuffer.h"

#ifndef LOG_WRITER
	#define LOG_WRITER STDOUT
#endif

#ifdef LOG_WRITER
	#ifndef LOG_ROLLING_SIZE
		#if LLOG_WRITER == 	ROLLING_FILE
			#define LOG_ROLLING_SIZE 24
		#endif
	#endif
#endif

#ifndef LOG_FILE
	#define LOG_FILE "~/nets/nets.log"
#endif

namespace nets
{
	namespace base
	{
		enum LogWriter
		{
			STDOUT = 0,
			SINGLE_FILE,
			DAILY_FILE,
			ROLLING_FILE
		};

		class ILogWriter
		{
			protected:
				virtual ~ILogWriter() = default;

			public:
				virtual void write(const LogBuffer& logBuffer) = 0;
		};

		DECLARE_SINGLETON_CLASS(StdoutLogWriter), public ILogWriter
		{
			DEFINE_SINGLETON(StdoutLogWriter)

			public:
				void write(const LogBuffer& logBuffer) override;
		};

		class FileLogWriter : public ILogWriter
		{
			public:
				void write(const LogBuffer& logBuffer) override
				{
				}

			private:
				::std::list<LogBuffer> bufferList_;
				::std::thread ioThread_ {};
		};

		DECLARE_SINGLETON_CLASS(SingleFileLogWriter), public FileLogWriter
		{
			DEFINE_SINGLETON(SingleFileLogWriter)

			public:
				void write(const LogBuffer& logBuffer) override;
		};

		DECLARE_SINGLETON_CLASS(DailyFileLogWriter), public FileLogWriter
		{
			DEFINE_SINGLETON(DailyFileLogWriter)

			public:
				void write(const LogBuffer& logBuffer) override;
		};

		DECLARE_SINGLETON_CLASS(RollingFileLogWriter), public FileLogWriter
		{
			DEFINE_SINGLETON(RollingFileLogWriter)

			public:
				void write(const LogBuffer& logBuffer) override;
		};


		DECLARE_SINGLETON_CLASS(LogWriterFactory)
		{
			DEFINE_SINGLETON(LogWriterFactory)

			public:
				ILogWriter* getLogWriter() const;
		};
	} // namespace base
} // namespace nets

#define LOG_WRITER_FACTORY (LogWriterFactory::getInstance())

#endif //NETS_LOGWRITER_H
