//
// Created by guang19 on 2022/8/4.
//

#include "nets/base/exception/AbstractException.h"

#include <cxxabi.h>
#include <execinfo.h>

namespace nets::base
{
    AbstractException::AbstractException() : stackTrace_()
    {
        backTrace();
    }

    void AbstractException::backTrace() noexcept
    {
        static constexpr size_t MaxFrameSize = 128;
        size_t FunctionNameLen = 256;
        void* addrList[MaxFrameSize];
        int32_t addrLen = ::backtrace(addrList, MaxFrameSize);
        if (addrLen == 0)
        {
            return;
        }
        char** symbolList = ::backtrace_symbols(addrList, addrLen);
        if (symbolList != nullptr)
        {
            char* functionName = static_cast<char*>(::malloc(FunctionNameLen));
            for (int32_t i = 1; i < addrLen; ++i)
            {
                char *begin = nullptr, *offset = nullptr, *end = nullptr;
                for (char* p = symbolList[i]; *p; ++p)
                {
                    if ('(' == *p)
                    {
                        begin = p;
                    }
                    else if ('+' == *p)
                    {
                        offset = p;
                    }
                    else if (')' == *p && offset != nullptr)
                    {
                        end = p;
                        break;
                    }
                }
                if (begin != nullptr && offset != nullptr && end != nullptr && begin < offset)
                {
                    *begin++ = '\0';
                    *offset++ = '\0';
                    *end = '\0';
                    int32_t status = 0;
                    char* ret = abi::__cxa_demangle(begin, functionName, &FunctionNameLen, &status);
                    if (status == 0)
                    {
                        functionName = ret;
                        stackTrace_.push_back('\t');
                        stackTrace_.append(symbolList[i]);
                        stackTrace_.append(functionName);
                        stackTrace_.push_back('+');
                        stackTrace_.append(offset);
                        stackTrace_.push_back('\n');
                    }
                    else
                    {
                        stackTrace_.push_back('\t');
                        stackTrace_.append(symbolList[i]);
                        stackTrace_.push_back(':');
                        stackTrace_.append(begin);
                        stackTrace_.append("()+");
                        stackTrace_.append(offset);
                        stackTrace_.push_back('\n');
                    }
                }
                else
                {
                    stackTrace_.append(symbolList[i]);
                    stackTrace_.push_back('\n');
                }
            }
            free(functionName);
            free(symbolList);
        }
    }
} // namespace nets::base