//
// Created by guang19
//

#ifndef NETS_BASE_LOG_FILE_H
#define NETS_BASE_LOG_FILE_H

#include <string>

#include "nets/base/Noncopyable.h"

namespace nets::base
{
    class LogFile : Noncopyable
    {
    public:
        using SizeType = ::size_t;
        using TimeType = ::time_t;
        using StringType = ::std::string;

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
        void getFileInfo(uint64_t* fileSize, ::time_t* createTime);

    private:
        FILE* fp_ {nullptr};
        StringType dir_ {};
        StringType file_ {};
        SizeType bytes_ {0};
        TimeType lastRollTime_ {0};

        static constexpr ::size_t FileIoBufferSize = 1024 * 256;
        char buffer_[FileIoBufferSize] {0};
    };

} // namespace nets::base

#endif // NETS_BASE_LOG_FILE_H