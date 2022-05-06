//
// Created by YangGuang on 2022/5/7.
//

#include "nets/base/log/LogMessage.h"

#include <cstring>

namespace nets
{
	namespace base
	{
		LogMessage_::LogMessage_(LogLevel logLevel, const char* file, uint32_t line) :
			logTime_(Timestamp::now()), logLevel_(logLevel), line_(line), message_()
		{
			filename_ = basename(file);
		}
	} // namespace base
} // namespace nets