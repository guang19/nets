//
// Created by YangGuang on 2022/4/3.
//

#ifndef NETS_BASE_LOG_BUFFER_STREAM_H
#define NETS_BASE_LOG_BUFFER_STREAM_H

#include <string>

#include "nets/base/StackBuffer.h"

namespace nets::base
{
    class LogBufferStream : Noncopyable
    {
    private:
        // log length limit: 2K
        static constexpr IntType DefaultLogBufferSize = (1024 << 1);
        using LogBuffer = StackBuffer<DefaultLogBufferSize>;

    public:
        LogBufferStream() = default;
        ~LogBufferStream() = default;

    public:
        inline const LogBuffer& buffer() const
        {
            return buffer_;
        }

    public:
        LogBufferStream& operator<<(char c);
        LogBufferStream& operator<<(int8_t n);
        LogBufferStream& operator<<(uint8_t n);
        LogBufferStream& operator<<(int16_t n);
        LogBufferStream& operator<<(uint16_t n);
        LogBufferStream& operator<<(int32_t n);
        LogBufferStream& operator<<(uint32_t n);
        LogBufferStream& operator<<(int64_t n);
        LogBufferStream& operator<<(uint64_t n);
        LogBufferStream& operator<<(const void* ptr);
        LogBufferStream& operator<<(float f);
        LogBufferStream& operator<<(double f);
        LogBufferStream& operator<<(const char* str);
        LogBufferStream& operator<<(const ::std::string& str);
        LogBufferStream& operator<<(const ::std::string_view& str);
        LogBufferStream& operator<<(const LogBufferStream& stream);

    private:
        LogBuffer buffer_ {};
    };
} // namespace nets::base

#endif // NETS_BASE_LOG_BUFFER_STREAM_H
