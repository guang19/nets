//
// Created by YangGuang on 2022/4/17.
//

#include "base/log/LogAppender.h"

namespace nets
{
	namespace base
	{
		INIT_SINGLETON(StdoutLogAppender)

		INIT_SINGLETON(FileLogAppender)

		void StdoutLogAppender::append(const nets::base::LogBuffer &logBuffer)
		{
			fwrite(logBuffer.getBuffer(),1, logBuffer.length(), ::stdout);
			fflush(::stdout);
		}

		void FileLogAppender::append(const nets::base::LogBuffer &logBuffer)
		{

		}
	} // namespace base
} // namespace nets
