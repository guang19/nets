//
// Created by YangGuang on 2022/4/16.
//

#ifndef NETS_LOGGERMANAGER_H
#define NETS_LOGGERMANAGER_H

#include "base/log/LogAppender.h"

#ifndef LOG_LEVEL
	#define LOG_LEVEL DEBUG
#endif

#ifndef LOG_APPENDER
	#define LOG_APPENDER STDOUT
#endif

#ifndef LOG_PATH
	#define LOG_PATH "~/nets/log"
#endif

#ifndef LOG_ROLLING_SIZE
	#define LOG_ROLLING_SIZE 30
#endif

namespace nets
{
	namespace base
	{
		enum LogLevel
		{
			TRACE = 0,
			DEBUG,
			INFO,
			WARN,
			ERROR,
			FATAL,
			NUM_OF_LOG_LEVELS = 6
		};

		DECLARE_SINGLETON_CLASS(LoggerManager)
		{
			DEFINE_SINGLETON(LoggerManager)

			public:
				inline LogLevel getLogLevel() const
				{
					return GLogLevel;
				}

				inline ILogAppender* getLogAppender() const
				{
					switch (GLogAppender)
					{
						case STDOUT:
						{
							return StdoutLogApdr;
						}
						case FILE:
						{
							return FileLogApdr;
						}
						default:
							return StdoutLogApdr;
					}
				}

				inline ::std::string getLogPath() const
				{
					return GLogPath;
				}

				inline uint32_t getRollingFileSize() const
				{
					return GRollingFileSize;
				}

			private:
				LogLevel GLogLevel { LOG_LEVEL };
				LogAppender GLogAppender { LOG_APPENDER };
				::std::string GLogPath { LOG_PATH };
				uint32_t GRollingFileSize { LOG_ROLLING_SIZE };
		};
	} // namespace base
}	// namespace nets

#define LogMgr (LoggerManager::getInstance())

#endif //NETS_LOGGERMANAGER_H
