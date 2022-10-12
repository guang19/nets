// MIT License
//
// Copyright (c) 2022 guang19
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

// @brief common macro and function

#include "nets/base/Common.h"

#include <execinfo.h>

namespace nets
{
    StringType stackTrace()
    {
        constexpr size_t kMaxBackTraceSize = 24;
        StringType backtraceInfo;
        void* addrList[kMaxBackTraceSize];
        ::int32_t numOfAddr = ::backtrace(addrList, kMaxBackTraceSize);
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
} // namespace nets