//
// Created by YangGuang on 2022/4/17.
//

#include "base/log/LogWriter.h"

namespace nets
{
	namespace base
	{
		INIT_SINGLETON(StdoutLogWriter)
		INIT_SINGLETON(SingleFileLogWriter)
		INIT_SINGLETON(DailyFileLogWriter)
		INIT_SINGLETON(RollingFileLogWriter)
		INIT_SINGLETON(LogWriterFactory)

		void StdoutLogWriter::write(const LogBuffer& logBuffer)
		{
			fwrite(logBuffer.getBuffer(),1, logBuffer.length(), ::stdout);
			fflush(::stdout);
		}

		void SingleFileLogWriter::write(const LogBuffer& logBuffer)
		{
		}

		void DailyFileLogWriter::write(const LogBuffer& logBuffer)
		{
		}

		void RollingFileLogWriter::write(const LogBuffer& logBuffer)
		{
		}

		ILogWriter* LogWriterFactory::getLogWriter() const
		{
			switch (LOG_WRITER)
			{
				case LogWriter::STDOUT:
					return StdoutLogWriter::getInstance();
				case LogWriter::SINGLE_FILE:
					return SingleFileLogWriter::getInstance();
				case LogWriter::DAILY_FILE:
					return DailyFileLogWriter::getInstance();
				case LogWriter::ROLLING_FILE:
					return RollingFileLogWriter::getInstance();
				default:
					return nullptr;
			}
		}
	} // namespace base
} // namespace nets
