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

// @brief The encapsulation of log file operations

#ifndef NETS_LOG_FILE_H
#define NETS_LOG_FILE_H

#include <string>

#include "nets/base/Noncopyable.h"
#include "nets/base/Types.h"

namespace nets
{
    class LogFile : Noncopyable
    {
    public:
        explicit LogFile(const char* file);
        ~LogFile();

    public:
        void write(const char* data, SizeType length);
        void flush();

        void renameByNowTime(TimeType now);

        void mkdirR(const char* multiLevelDir);

        inline SizeType size() const
        {
            return bytes_;
        }

        inline TimeType lastRollTime() const
        {
            return lastRollTime_;
        }

    private:
        void getFileInfo(SizeType* fileSize, TimeType* createTime);

    private:
        FILE* fp_;
        StringType dir_;
        StringType file_;
        SizeType bytes_;
        TimeType lastRollTime_;

        static constexpr SizeType kFileIoBufferSize = 1024 * 256;
        char buffer_[kFileIoBufferSize] {0};
    };

} // namespace nets

#endif // NETS_LOG_FILE_H