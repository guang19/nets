//
// Created by guang19
//

#include "nets/base/log/LogAppender.h"

namespace nets
{
    INIT_SINGLETON(StdoutLogAppender);

    void StdoutLogAppender::append(const LogBuffer& logBuffer)
    {
        ::fwrite(logBuffer.array(), sizeof(char), logBuffer.length(), stdout);
    }
} // namespace nets