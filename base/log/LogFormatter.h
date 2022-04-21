//
// Created by YangGuang on 2022/4/21.
//

#ifndef NETS_LOGFORMATTER_H
#define NETS_LOGFORMATTER_H

#include "base/log/Logging.h"
#include "base/log/LogBuffer.h"

namespace nets
{
	namespace base
	{
		class LogFormatter
		{
			public:
				virtual void formatLogMessage(LogBuffer& logBufferStream, LogMessage& logMessage) = 0;
		};

		class DefaultLogFormatter : public LogFormatter
		{
			public:
				void formatLogMessage(LogBuffer& logBufferStream, LogMessage& logMessage) override;
		};

	} // namespace base
} // namespace nets

#endif //NETS_LOGFORMATTER_H
