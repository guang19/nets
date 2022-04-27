//
// Created by YangGuang on 2022/4/21.
//

#ifndef NETS_LOGFORMATTER_H
#define NETS_LOGFORMATTER_H

#include "base/log/LogBuffer.h"
#include "base/log/Logging.h"
#include "base/Singleton.h"

namespace nets
{
	namespace base
	{
		class ILogFormatter
		{
			protected:
				virtual ~ILogFormatter() = default;

			public:
				virtual void formatLogMessage(LogBuffer& logBufferStream, LogMessage& logMessage) = 0;
		};

		DECLARE_SINGLETON_CLASS(DefaultLogFormatter), public ILogFormatter
		{
			DEFINE_SINGLETON(DefaultLogFormatter)

			public:
				void formatLogMessage(LogBuffer& logBufferStream, LogMessage& logMessage) override;
		};

		DECLARE_SINGLETON_CLASS(LogFormatterFactory)
		{
			DEFINE_SINGLETON(LogFormatterFactory)

			public:
				ILogFormatter* getLogFormatter() const;
		};
	} // namespace base
} // namespace nets

#define LOG_FORMATTER (nets::base::LogFormatterFactory::getInstance()->getLogFormatter())

#endif //NETS_LOGFORMATTER_H
