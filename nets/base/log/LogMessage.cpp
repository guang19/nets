//
// Created by YangGuang on 2022/5/7.
//

#include "nets/base/log/LogMessage.h"

#include <cstring>

namespace nets::base
{
    LogMessage::LogMessage(LogLevel logLevel, const char* file, uint32_t line)
        : logTime_(Timestamp::now()), logLevel_(logLevel), line_(line), stream_()
    {
        filename_ = ::strrchr(file, '/');
        if (filename_ != nullptr)
        {
            filename_ = filename_ + 1;
        }
    }
} // namespace nets::base