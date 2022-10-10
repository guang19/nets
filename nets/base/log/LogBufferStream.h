//
// Created by guang19
//

#ifndef NETS_LOG_BUFFER_STREAM_H
#define NETS_LOG_BUFFER_STREAM_H

#include "nets/base/StackBuffer.h"

namespace nets
{
    class LogBufferStream : Noncopyable
    {
    private:
        // log length limit: 2K
        static constexpr SizeType kLogBufferSize = (1024 << 1);
        using LogBuffer = StackBuffer<kLogBufferSize>;

    public:
        LogBufferStream();
        ~LogBufferStream() = default;

    public:
        inline const LogBuffer& buffer() const
        {
            return buffer_;
        }

    public:
        LogBufferStream& operator<<(char value);
        LogBufferStream& operator<<(::int8_t value);
        LogBufferStream& operator<<(::uint8_t value);
        LogBufferStream& operator<<(::int16_t value);
        LogBufferStream& operator<<(::uint16_t value);
        LogBufferStream& operator<<(::int32_t value);
        LogBufferStream& operator<<(::uint32_t value);
        LogBufferStream& operator<<(::int64_t value);
        LogBufferStream& operator<<(::uint64_t value);
        LogBufferStream& operator<<(const void* ptr);
        LogBufferStream& operator<<(float value);
        LogBufferStream& operator<<(double value);
        LogBufferStream& operator<<(const char* str);
        LogBufferStream& operator<<(const ::std::string& str);
        LogBufferStream& operator<<(const ::std::string_view& str);
        LogBufferStream& operator<<(const LogBufferStream& stream);

    private:
        LogBuffer buffer_;
    };
} // namespace nets

#endif // NETS_LOG_BUFFER_STREAM_H