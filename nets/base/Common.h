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

#include <cstdarg>
#include <cstdio>
#include <cstring>

#include "nets/base/exception/FileCreateException.h"
#include "nets/base/exception/FileOpenException.h"

namespace nets
{
    namespace
    {
        using StringType = ::std::string;
    }

    StringType stackTrace();

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
} // namespace nets

#define UNUSED(X) ((void) (X))

#define MEMZERO(P, LEN) (::memset((P), 0, (LEN)))

#define CHECK_CLASS_COMPLETE_TYPE(CLASS)                                                                                    \
    do                                                                                                                      \
    {                                                                                                                       \
        typedef char CLASS_MUST_BE_COMPLETE_TYPE[sizeof(CLASS) == 0 ? -1 : 1];                                              \
        CLASS_MUST_BE_COMPLETE_TYPE jugg;                                                                                   \
        UNUSED(jugg);                                                                                                       \
    } while (0)

#define STACK_TRACE (stackTrace())

#define THROW_FMT(EXCEPTION, FMT, ...) (nets::throwFmt<EXCEPTION>(FMT, ##__VA_ARGS__))

#define THROW_FILE_OPEN_EXCEPTION(ERRNUM)                                                                                   \
    switch (ERRNUM)                                                                                                         \
    {                                                                                                                       \
        case ENOENT:                                                                                                        \
            THROW_FMT(FileOpenException, "no such file");                                                                   \
            break;                                                                                                          \
        case EACCES:                                                                                                        \
        case EPERM:                                                                                                         \
            THROW_FMT(FileOpenException, "no permission to access file");                                                   \
            break;                                                                                                          \
        case EINVAL:                                                                                                        \
            THROW_FMT(FileOpenException, "invalid value in flags");                                                         \
            break;                                                                                                          \
        case EMFILE:                                                                                                        \
            THROW_FMT(FileOpenException, "the per-process limit on the number of open file descriptors has been reached");  \
            break;                                                                                                          \
        case ENAMETOOLONG:                                                                                                  \
            THROW_FMT(FileOpenException, "pathname was too long");                                                          \
            break;                                                                                                          \
        default:                                                                                                            \
            THROW_FMT(FileOpenException, "failed to open file for unknown reason,errno=%d", ERRNUM);                        \
            break;                                                                                                          \
    }

#define THROW_FILE_CREATE_EXCEPTION(ERRNUM)                                                                                 \
    switch (ERRNUM)                                                                                                         \
    {                                                                                                                       \
        case ENOENT:                                                                                                        \
            THROW_FMT(FileCreateException, "no such file");                                                                 \
            break;                                                                                                          \
        case EACCES:                                                                                                        \
        case EPERM:                                                                                                         \
            THROW_FMT(FileCreateException, "no permission to create file");                                                 \
            break;                                                                                                          \
        case EINVAL:                                                                                                        \
            THROW_FMT(FileCreateException, "invalid value in flags");                                                       \
            break;                                                                                                          \
        case EMFILE:                                                                                                        \
            THROW_FMT(FileCreateException,                                                                                  \
                      "the per-process limit on the number of open file descriptors has been reached");                     \
            break;                                                                                                          \
        case ENAMETOOLONG:                                                                                                  \
            THROW_FMT(FileCreateException, "path name was too long");                                                       \
            break;                                                                                                          \
        default:                                                                                                            \
            THROW_FMT(FileCreateException, "failed to create file for unknown reason,errno=%d", ERRNUM);                    \
            break;                                                                                                          \
    }

#endif // NETS_COMMON_MACRO_H