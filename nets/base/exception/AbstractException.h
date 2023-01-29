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

// @brief Base class for all custom exceptions

#ifndef NETS_ABSTRACT_EXCEPTION_H
#define NETS_ABSTRACT_EXCEPTION_H

#include <cstdarg>
#include <exception>

#include "nets/base/Noncopyable.h"
#include "nets/base/Types.h"

// throw an exception with formatter
#define THROW_FMT(EXCEPTION, FMT, ...) (nets::throwFmt<EXCEPTION>(#EXCEPTION ":" FMT, ##__VA_ARGS__))

namespace nets
{
    template <class E>
    void throwFmt(const char* fmt, ...)
    {
        char msgBuf[255] = {0};
        va_list vl;
        va_start(vl, fmt);
        ::vsnprintf(msgBuf, sizeof(msgBuf), fmt, vl);
        va_end(vl);
        throw E(msgBuf);
    }

    class AbstractException : public ::std::exception, Noncopyable
    {
    protected:
        AbstractException();
        explicit AbstractException(const StringType& cause);
        ~AbstractException() override = default;

    public:
        inline const char* what() const noexcept override
        {
            return cause_.c_str();
        }

        inline const StringType& backtrace() const noexcept
        {
            return backtraceInfo_;
        }

    protected:
        StringType cause_;
        StringType backtraceInfo_;
    };
} // namespace nets

#endif // NETS_ABSTRACT_EXCEPTION_H