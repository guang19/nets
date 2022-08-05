//
// Created by guang19 on 2022/6/10.
//

#include "nets/base/log/LogFile.h"

#include <stdexcept>
#include <sys/stat.h>
#include <unistd.h>

#include "nets/base/CommonMacro.h"

namespace nets::base
{
    namespace
    {
        constexpr uint32_t MaxFilePathLength = 512;
        const char* const LogFileNamePattern = "%Y-%m-%d_%H-%M-%S";
        const char* const LogFileSuffix = ".log";
    } // namespace

    LogFile::LogFile(const char* file) : fp_(nullptr), dir_(), file_(file), bytes_(0), lastRollTime_(0)
    {
        SizeType filePathLength = file_.length();
        if (filePathLength > MaxFilePathLength || filePathLength <= 0)
        {
            THROW_FMT(::std::invalid_argument, "log file name length %lu more than %u", filePathLength, MaxFilePathLength);
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
            int32_t errNum = errno;
            THROW_FILE_OPEN_EXCEPTION(errNum);
        }
        getFileInfo(&bytes_, &lastRollTime_);
        ::setbuffer(fp_, buffer_, FileIoBufferSize);
    }

    LogFile::~LogFile()
    {
        if (fp_ != nullptr)
        {
            ::fclose(fp_);
            fp_ = nullptr;
        }
    }

    void LogFile::write(const char* data, SizeType len)
    {
        uint64_t writtenBytes = 0;
        while (writtenBytes < len)
        {
            uint64_t remain = len - writtenBytes;
            // not thread-safe
            uint64_t n = ::fwrite_unlocked(data + writtenBytes, 1, remain, fp_);
            if (n != remain)
            {
                int32_t errNum = ::ferror(fp_);
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
        struct tm tmS {};
        ::localtime_r(&now, &tmS);
        char newFilename[32] = {0};
        ::strftime(newFilename, sizeof(newFilename), LogFileNamePattern, &tmS);
        ::strcat(newFilename, LogFileSuffix);
        if (!dir_.empty())
        {
            char tmpFile[MaxFilePathLength] = {0};
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
            int32_t errNum = errno;
            THROW_FILE_OPEN_EXCEPTION(errNum);
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
        SizeType len = ::strlen(multiLevelDir);
        char tmpDir[MaxFilePathLength] = {0};
        char* dirPtr = tmpDir;
        char path[MaxFilePathLength] = {0};
        MEMZERO(tmpDir, MaxFilePathLength);
        MEMZERO(path, MaxFilePathLength);
        ::memcpy(tmpDir, multiLevelDir, len);
        char* spStr = nullptr;
        while (nullptr != (spStr = ::strsep(&dirPtr, "/")))
        {
            if (::strlen(spStr) > 0)
            {
                ::strcat(path, "/");
                ::strcat(path, spStr);
                if (0 != ::access(path, F_OK))
                {
                    if (0 != ::mkdir(path, 0775))
                    {
                        int32_t errNum = errno;
                        THROW_FILE_CREATE_EXCEPTION(errNum);
                    }
                }
            }
        }
    }

    void LogFile::getFileInfo(SizeType* fileSize, TimeType* createTime)
    {
        struct stat fileInfo {};
        if (0 != ::fstat(::fileno(fp_), &fileInfo))
        {
            return;
        }
        if (fileSize != nullptr)
        {
            *fileSize = fileInfo.st_size;
        }
        if (createTime != nullptr)
        {
            *createTime = fileInfo.st_ctime;
        }
    }
} // namespace nets::base