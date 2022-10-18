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

#include "nets/base/log/LogFile.h"

#include <ctime>
#include <stdexcept>
#include <sys/stat.h>
#include <unistd.h>

#include "nets/base/Common.h"
#include "nets/base/exception/FileCreateException.h"
#include "nets/base/exception/FileOpenException.h"

namespace nets
{
    namespace
    {
        constexpr Int32Type kMaxFilePathLength = 512;
        constexpr char const kLogFileNamePattern[] = "%Y-%m-%d_%H-%M-%S";
        constexpr char const kLogFileSuffix[] = ".log";
    } // namespace

    LogFile::LogFile(const char* file) : fp_(nullptr), dir_(), file_(file), bytes_(0), lastRollTime_(0)
    {
        SizeType filePathLength = file_.length();
        if (filePathLength > kMaxFilePathLength || filePathLength == 0)
        {
            THROW_FMT(::std::invalid_argument,
                      "the log file cannot be empty and the length of the log file name cannot exceed %d",
                      kMaxFilePathLength);
        }
        SizeType lastIndex = file_.find_last_of('/');
        if (lastIndex != StringType::npos)
        {
            dir_ = StringType(file_, 0, lastIndex + 1);
            // create parent directory of log file
            mkdirR(dir_.c_str());
        }
        if (nullptr == (fp_ = ::fopen(file, "ae")))
        {
            THROW_FILE_OPEN_EXCEPTION(errno)
        }
        getFileInfo(&bytes_, &lastRollTime_);
        MEMZERO(buffer_, sizeof(buffer_));
        ::setbuffer(fp_, buffer_, kFileIoBufferSize);
    }

    LogFile::~LogFile()
    {
        if (fp_ != nullptr)
        {
            ::fclose(fp_);
            fp_ = nullptr;
        }
    }

    void LogFile::write(const char* data, SizeType length)
    {
        SizeType writtenBytes = 0L;
        while (writtenBytes < length)
        {
            SizeType remain = length - writtenBytes;
            // not thread-safe
            SizeType n = ::fwrite_unlocked(data + writtenBytes, 1, remain, fp_);
            if (n != remain)
            {
                Int32Type errNum = ::ferror(fp_);
                if (errNum != 0)
                {
                    ::fprintf(stderr,
                              "log file write error: "
                              "\"%s\""
                              "\n",
                              ::strerror(errNum));
                    break;
                }
            }
            writtenBytes += n;
        }
        bytes_ += writtenBytes;
    }

    void LogFile::flush()
    {
        ::fflush(fp_);
    }

    void LogFile::renameByNowTime(TimeType now)
    {
        if (fp_ != nullptr)
        {
            ::fclose(fp_);
            fp_ = nullptr;
        }
        using Tm = struct tm;
        Tm tms {};
        ::localtime_r(&now, &tms);
        char newFilename[32] = {0};
        ::strftime(newFilename, sizeof(newFilename), kLogFileNamePattern, &tms);
        ::strcat(newFilename, kLogFileSuffix);
        if (!dir_.empty())
        {
            char tmpFile[kMaxFilePathLength] = {0};
            ::strcat(tmpFile, dir_.c_str());
            ::strcat(tmpFile, newFilename);
            ::rename(file_.c_str(), tmpFile);
        }
        else
        {
            ::rename(file_.c_str(), newFilename);
        }
        if (nullptr == (fp_ = ::fopen(file_.c_str(), "ae")))
        {
            THROW_FILE_OPEN_EXCEPTION(errno)
        }
        getFileInfo(&bytes_, nullptr);
        lastRollTime_ = now;
    }

    void LogFile::mkdirR(const char* multiLevelDir)
    {
        if (0 == ::access(multiLevelDir, F_OK))
        {
            return;
        }
        SizeType length = ::strlen(multiLevelDir);
        char tmpDir[kMaxFilePathLength] = {0};
        char* dirPtr = tmpDir;
        char path[kMaxFilePathLength] = {0};
        MEMZERO(tmpDir, kMaxFilePathLength);
        MEMZERO(path, kMaxFilePathLength);
        ::memcpy(tmpDir, multiLevelDir, length);
        char* spStr = nullptr;
        while (nullptr != (spStr = ::strsep(&dirPtr, "/")))
        {
            if (::strlen(spStr) > 0)
            {
                ::strcat(path, "/");
                ::strcat(path, spStr);
                if (::access(path, F_OK) == -1)
                {
                    if (::mkdir(path, 0775) == -1)
                    {
                        THROW_FILE_CREATE_EXCEPTION(errno)
                    }
                }
            }
        }
    }

    void LogFile::getFileInfo(SizeType* fileSize, TimeType* createTime)
    {
        using FileState = struct stat;
        FileState fileInfo {};
        if (::fstat(::fileno(fp_), &fileInfo) != -1)
        {
            if (fileSize != nullptr)
            {
                *fileSize = fileInfo.st_size;
            }
            if (createTime != nullptr)
            {
                *createTime = fileInfo.st_ctime;
            }
        }
    }
} // namespace nets