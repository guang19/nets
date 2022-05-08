//
// Created by YangGuang on 2022/5/7.
//

#ifndef NETS_LOGMESSAGE_H
#define NETS_LOGMESSAGE_H

#include "nets/base/log/LogBuffer.h"
#include "nets/base/Noncopyable.h"
#include "nets/base/Timestamp.h"

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

		typedef struct LogMessage_ : Noncopyable
		{
			public:
				explicit LogMessage_(LogLevel logLevel, const char* file, uint32_t line);
				~LogMessage_() = default;

			public:
				inline const Timestamp& getLogTime() const
				{
					return logTime_;
				}

				inline LogLevel getLogLevel() const
				{
					return logLevel_;
				}

				inline const char* getFilename() const
				{
					return filename_;
				}

				inline uint32_t getLine() const
				{
					return line_;
				}

				inline LogBuffer& getStream()
				{
					return stream_;
				}

			private:
				Timestamp logTime_ {};
				LogLevel logLevel_ {};
				const char* filename_ { nullptr };
				uint32_t line_ { 0 };
				LogBuffer stream_ {};
		} LogMessage;
	} // namespace base
} // namespace nets

#endif //NETS_LOGMESSAGE_H
