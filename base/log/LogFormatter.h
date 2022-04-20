//
// Created by YangGuang on 2022/4/21.
//

#ifndef NETS_LOGFORMATTER_H
#define NETS_LOGFORMATTER_H

#include "base/log/Logging.h"
#include "base/log/LogBufferStream.h"

namespace nets
{
	namespace base
	{
		class LogFormatter
		{
			public:
				virtual void formatLogMessage(LogBufferStream& logBufferStream, LogMessage& logMessage) = 0;
		};

		class DefaultLogFormatter : public LogFormatter
		{
			public:
				void formatLogMessage(LogBufferStream& logBufferStream, LogMessage& logMessage) override;
		};

	} // namespace base
} // namespace nets

#endif //NETS_LOGFORMATTER_H
