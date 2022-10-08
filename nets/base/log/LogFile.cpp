//
// Created by guang19
//

#include "nets/base/log/LogFile.h"

#include <ctime>
#include <stdexcept>
#include <sys/stat.h>
#include <unistd.h>

#include "nets/base/CommonMacro.h"

namespace nets
{
    namespace
    {
        constexpr ::int32_t gMaxFilePathLength = 512;
        constexpr char const gLogFileNamePattern[] = "%Y-%m-%d_%H-%M-%S";
        constexpr char const gLogFileSuffix[] = ".log";
    } // namespace

    LogFile::LogFile(const char* file) : fp_(nullptr), dir_(), file_(file), bytes_(0), lastRollTime_(0)
    {
        SizeType filePathLength = file_.length();
        if (filePathLength > gMaxFilePathLength || filePathLength == 0)
        {
            THROW_FMT(::std::invalid_argument, "log file name length %lu more than %u", filePathLength, gMaxFilePathLength);
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
        ::setbuffer(fp_, buffer_, gFileIoBufferSize);
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
                ::int32_t errNum = ::ferror(fp_);
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
        ::strftime(newFilename, sizeof(newFilename), gLogFileNamePattern, &tms);
        ::strcat(newFilename, gLogFileSuffix);
        if (!dir_.empty())
        {
            char tmpFile[gMaxFilePathLength] = {0};
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
        char tmpDir[gMaxFilePathLength] = {0};
        char* dirPtr = tmpDir;
        char path[gMaxFilePathLength] = {0};
        MEMZERO(tmpDir, gMaxFilePathLength);
        MEMZERO(path, gMaxFilePathLength);
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