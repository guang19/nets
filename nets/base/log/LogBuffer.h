//
// Created by guang19
//

#ifndef NETS_LOG_BUFFER_H
#define NETS_LOG_BUFFER_H

#include "nets/base/StackBuffer.h"

namespace nets
{
    namespace
    {
        // log length limit: 4K
        constexpr SizeType kLogBufferSize = (1024 << 2);
        using LogBuffer = StackBuffer<kLogBufferSize>;
    }

    class LogBufferStream : Noncopyable
    {
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

#endif // NETS_LOG_BUFFER_H