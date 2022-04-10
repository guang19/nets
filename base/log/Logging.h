//
// Created by YangGuang on 2022/4/1.
//

#ifndef NETS_LOGGING_H
#define NETS_LOGGING_H

#include <string>
#include <string.h>
#include "base/Noncopyable.h"
#include "base/ThreadHelper.h"
#include "base/log/Timestamp.h"

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

		class Logger : Noncopyable
		{
			private:
				typedef struct LogLineComponent_
				{
					public:
						Timestamp time_;
						LogLevel logLevel_;
						const char* sourcefile_;
						const char* function_;
						int32_t line_;

						explicit LogLineComponent_(const Timestamp& time, LogLevel logLevel,
												   const char* sourcefile, const char* function,
												   int32_t line) :
												   time_(time), logLevel_(logLevel), function_(function), line_(line)
						{
							const char* basename = strrchr(sourcefile, '/');
							if (basename != nullptr)
							{
								sourcefile_ = basename + 1;
							}
							else
							{
								sourcefile_ = sourcefile;
							}
						}
				} LogLineComponent;

			public:
				Logger() = default;
				~Logger() = default;

			public:
				void log(LogLevel logLevel, const char* sourcefile, const char* function, int32_t line)
				{
					LogLineComponent logLineComponent {
						Timestamp::now(),
						logLevel,
						sourcefile,
						function,
						line
					};
					switch (logLevel)
					{
						case LogLevel::TRACE:
						{
							trace(logLineComponent);
							break;
						}
						default:
						{
							break;
						}
					}
				}

				void trace(const LogLineComponent& logLineComponent);

				void debug(const LogLineComponent& logLineComponent)
				{

				}

				void info(const LogLineComponent& logLineComponent)
				{

				}

				void warn(const LogLineComponent& logLineComponent)
				{

				}

				void error(const LogLineComponent& logLineComponent)
				{

				}

				void fatal(const LogLineComponent& logLineComponent)
				{

				}
		};
	} // namespace base
} // namespace nets

#endif //NETS_LOGGING_H
