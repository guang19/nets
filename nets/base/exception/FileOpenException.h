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

// @brief File open exception

#ifndef NETS_FILE_OPEN_EXCEPTION_H
#define NETS_FILE_OPEN_EXCEPTION_H

#include "nets/base/Common.h"
#include "nets/base/exception/AbstractException.h"

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

namespace nets
{
    class FileOpenException : public AbstractException
    {
    public:
        FileOpenException() : AbstractException() {}
        explicit FileOpenException(const StringType& cause) : AbstractException(cause) {}
        ~FileOpenException() override = default;
    };
} // namespace nets

#endif // NETS_FILE_OPEN_EXCEPTION_H