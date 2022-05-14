//
// Created by YangGuang on 2022/4/21.
//

#ifndef NETS_LOGFORMATTER_H
#define NETS_LOGFORMATTER_H

#include "nets/base/log/LogMessage.h"
#include "nets/base/Singleton.h"

namespace nets::base
{
	class ILogFormatter
	{
	protected:
		virtual ~ILogFormatter() = default;

	public:
		virtual void formatLogMessage(LogBuffer& logBuffer, LogMessage& logMessage) = 0;
	};

	DECLARE_SINGLETON_CLASS(DefaultLogFormatter), public ILogFormatter
	{
		DEFINE_SINGLETON(DefaultLogFormatter);

	public:
		void formatLogMessage(LogBuffer& logBuffer, LogMessage& logMessage) override;
	};

	class LogFormatterFactory
	{
	public:
		static ::std::shared_ptr<ILogFormatter> getLogFormatter();
	};
} // namespace nets::base

#define LOG_FORMATTER (nets::base::LogFormatterFactory::getLogFormatter())

#endif // NETS_LOGFORMATTER_H
