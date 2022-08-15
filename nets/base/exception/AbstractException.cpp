//
// Created by guang19
//

#include "nets/base/exception/AbstractException.h"

#include <execinfo.h>

namespace nets::base
{
    namespace
    {
        constexpr size_t MaxAddressSize = 128;
    }

    AbstractException::AbstractException() : cause_()
    {
        backTrace();
    }

    AbstractException::AbstractException(const StringType& cause) : cause_(cause)
    {
        backTrace();
    }

    void AbstractException::backTrace() noexcept
    {
        void* addrList[MaxAddressSize];
        int32_t addrLen = ::backtrace(addrList, MaxAddressSize);
        if (addrLen == 0)
        {
            return;
        }
        char** symbolList = ::backtrace_symbols(addrList, addrLen);
        if (symbolList != nullptr)
        {
            for (int32_t i = 1; i < addrLen; ++i)
            {
                stackTrace_.append(symbolList[i]);
                stackTrace_.push_back('\n');
            }
            free(symbolList);
        }
    }
} // namespace nets::base