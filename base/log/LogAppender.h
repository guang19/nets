//
// Created by YangGuang on 2022/4/17.
//

#ifndef NETS_LOGAPPENDER_H
#define NETS_LOGAPPENDER_H

#include "base/log/LogBufferStream.h"
#include "base/Singleton.h"

namespace nets
{
	namespace base
	{
		enum LogAppender
		{
			STDOUT = 0,
			FILE
		};

		class ILogAppender
		{
			protected:
				virtual ~ILogAppender() = default;

			public:
				virtual void append(const LogBufferStream& logBuffer) = 0;
		};

		DECLARE_SINGLETON_CLASS(StdoutLogAppender), public ILogAppender
		{
			DEFINE_SINGLETON(StdoutLogAppender)

			public:
				void append(const LogBufferStream &logBuffer) override;
		};

		DECLARE_SINGLETON_CLASS(FileLogAppender), public ILogAppender
		{
			DEFINE_SINGLETON(FileLogAppender)

			public:
				void append(const LogBufferStream &logBuffer) override;
		};
	} // namespace base
} // namespace nets

#define StdoutLogApdr (StdoutLogAppender::getInstance())
#define	FileLogApdr (FileLogAppender::getInstance())

#endif //NETS_LOGAPPENDER_H
