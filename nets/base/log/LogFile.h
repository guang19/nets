//
// Created by guang19
//

#ifndef NETS_LOG_FILE_H
#define NETS_LOG_FILE_H

#include <string>

#include "nets/base/Noncopyable.h"

namespace nets
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
        void getFileInfo(SizeType* fileSize, ::time_t* createTime);

    private:
        FILE* fp_ {nullptr};
        StringType dir_ {};
        StringType file_ {};
        SizeType bytes_ {0};
        TimeType lastRollTime_ {0};

        static constexpr ::size_t gFileIoBufferSize = 1024 * 256;
        char buffer_[gFileIoBufferSize] {0};
    };

} // namespace nets

#endif // NETS_LOG_FILE_H