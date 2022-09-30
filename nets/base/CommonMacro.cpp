//
// Created by guang19
//

#include "nets/base/CommonMacro.h"

#include <execinfo.h>

namespace nets::base
{
    StringType stackTrace()
    {
        {
            constexpr size_t gMaxBackTraceSize = 24;
            StringType backtraceInfo;
            void* addrList[gMaxBackTraceSize];
            ::int32_t numOfAddr = ::backtrace(addrList, gMaxBackTraceSize);
            if (numOfAddr == 0)
            {
                return backtraceInfo;
            }
            char** symbolList = ::backtrace_symbols(addrList, numOfAddr);
            if (symbolList != nullptr)
            {
                for (::int32_t i = 1; i < numOfAddr; ++i)
                {
                    backtraceInfo.append(symbolList[i]);
                    backtraceInfo.push_back('\n');
                }
                free(symbolList);
            }
            return backtraceInfo;
        }
    }
} // namespace nets::base