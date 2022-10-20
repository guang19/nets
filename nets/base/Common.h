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

#ifndef NETS_COMMON_MACRO_H
#define NETS_COMMON_MACRO_H

#include <algorithm>
#include <cstdarg>
#include <cstdio>
#include <cstring>

#include "nets/base/Types.h"

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

    struct AsciiCaseInsensitive
    {
        bool operator()(char lhs, char rhs) const
        {
            char k = lhs ^ rhs;
            if (k == 0)
            {
                return true;
            }
            if (k != 32)
            {
                return false;
            }
            k = lhs | rhs;
            return (k >= 'a' && k <= 'z');
        }
    };

    inline bool caseInsensitiveEqual(const StringType& s1, const StringType& s2)
    {
        if (s1.length() != s2.length())
        {
            return false;
        }
        else
        {
            return ::std::equal(s1.begin(), s1.end(), s2.begin(), AsciiCaseInsensitive());
        }
    }
} // namespace nets

#define UNUSED(X) ((void) (X))

#define MEMZERO(P, LEN) (::memset((P), 0, (LEN)))

#define ENUM_CLASS_TO_INT(X) (static_cast<::int32_t>(X))

#define CHECK_CLASS_COMPLETE_TYPE(CLASS)                                                                                    \
    do                                                                                                                      \
    {                                                                                                                       \
        typedef char CLASS_MUST_BE_COMPLETE_TYPE[sizeof(CLASS) == 0 ? -1 : 1];                                              \
        CLASS_MUST_BE_COMPLETE_TYPE jugg;                                                                                   \
        UNUSED(jugg);                                                                                                       \
    } while (0)

#define THROW_FMT(EXCEPTION, FMT, ...) (nets::throwFmt<EXCEPTION>(FMT, ##__VA_ARGS__))

#endif // NETS_COMMON_MACRO_H