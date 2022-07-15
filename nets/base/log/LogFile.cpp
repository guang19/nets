//
// Created by guang19 on 2022/6/10.
//

#include "nets/base/log/LogFile.h"

#include <cstdio>
#include <cstdlib>
#include <libgen.h>
#include <sys/stat.h>
#include <unistd.h>

#include "nets/base/CommonMacro.h"

namespace nets::base
{
    namespace
    {
        constexpr uint32_t MaxFilePathLen = 256;
        const char* const LogFileNamePattern = "%Y-%m-%d_%H-%M-%S";
        const char* const LogFileSuffix = ".log";
    } // namespace

    LogFile::LogFile(const char* file)
    {
        uint32_t filePathLen = ::strlen(file);
        if (filePathLen > MaxFilePathLen)
        {
            ::fprintf(stderr, "Error:log file name length more than %d\n", MaxFilePathLen);
            ::exit(1);
        }
        file_ = ::std::make_unique<char[]>(filePathLen);
        MEMZERO(file_.get(), filePathLen);
        ::memcpy(file_.get(), file, filePathLen);
        // <dirname> will modifies input str, so reallocate an array
        char tmpFile[MaxFilePathLen] = {0};
        ::memcpy(tmpFile, file, filePathLen);
        const char* dir = ::dirname(tmpFile);
        uint32_t dirLen = ::strlen(dir);
        dir_ = ::std::make_unique<char[]>(dirLen);
        MEMZERO(dir_.get(), dirLen);
        ::memcpy(dir_.get(), dir, dirLen);
        // log file has parent directory
        if (0 != ::strcmp(dir, "."))
        {
            // create parent directory of log file
            mkdirR(dir);
        }
        if (nullptr == (fp_ = ::fopen(file, "ae")))
        {
            ::fprintf(stderr, "Error:failed to open log file\n");
            ::exit(1);
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

    void LogFile::append(const char* data, SizeType len)
    {
        uint64_t writtenBytes = 0;
        while (writtenBytes < len)
        {
            uint64_t remain = len - writtenBytes;
            // not thread-safe
            uint64_t n = ::fwrite_unlocked(data + writtenBytes, 1, remain, fp_);
            if (n != remain)
            {
                int32_t err = ::ferror(fp_);
                if (err != 0)
                {
                    ::fprintf(stderr,
                              "Error:log file append error "
                              "\"%s\""
                              "\n",
                              ::strerror(err));
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
        char newFilename[26] = {0};
        MEMZERO(newFilename, sizeof(newFilename));
        ::strftime(newFilename, sizeof(newFilename), LogFileNamePattern, &tmS);
        ::strcat(newFilename, LogFileSuffix);
        if (::strcmp(dir_.get(), ".") != 0)
        {
            char tmpFile[MaxFilePathLen] = {0};
            ::strcat(tmpFile, dir_.get());
            ::strcat(tmpFile, "/");
            ::strcat(tmpFile, newFilename);
            ::rename(file_.get(), tmpFile);
        }
        else
        {
            ::rename(file_.get(), newFilename);
        }
        if (nullptr == (fp_ = ::fopen(file_.get(), "ae")))
        {
            ::fprintf(stderr, "Error:after rename log file,failed to open log file\n");
            ::exit(1);
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
        uint32_t len = ::strlen(multiLevelDir);
        char dir1[MaxFilePathLen] = {0};
        char* dirptr = dir1;
        char dir2[MaxFilePathLen] = {0};
        MEMZERO(dir1, MaxFilePathLen);
        MEMZERO(dir2, MaxFilePathLen);
        ::memcpy(dir1, multiLevelDir, len);
        char* spStr = nullptr;
        while (nullptr != (spStr = ::strsep(&dirptr, "/")))
        {
            if (::strlen(spStr) > 0)
            {
                ::strcat(dir2, "/");
                ::strcat(dir2, spStr);
                if (0 != ::access(dir2, F_OK))
                {
                    if (0 != ::mkdir(dir2, 0775))
                    {
                        ::fprintf(stderr, "Error: failed to create parent directory of log file\n");
                        ::exit(1);
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