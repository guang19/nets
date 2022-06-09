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
		virtual void formatLogMessage(LogMessage& logMessage, LogBufferStream& logBufferStream) = 0;
	};

	DECLARE_SINGLETON_CLASS(DefaultLogFormatter), public ILogFormatter
	{
		DEFINE_SINGLETON(DefaultLogFormatter);

	public:
		void formatLogMessage(LogMessage& logMessage, LogBufferStream& logBufferStream) override;
	};

	class LogFormatterFactory
	{
	public:
		static ::std::shared_ptr<ILogFormatter> getLogFormatter();
	};
} // namespace nets::base

#define LOG_FORMATTER (nets::base::LogFormatterFactory::getLogFormatter())

#endif // NETS_LOGFORMATTER_H
