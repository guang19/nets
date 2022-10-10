//
// Created by guang19
//

#include "nets/base/log/LogBuffer.h"

namespace nets
{
    LogBufferStream::LogBufferStream() : buffer_() {}

    LogBufferStream& LogBufferStream::operator<<(char value)
    {
        buffer_.writeBytes(&value, 1);
        return *this;
    }

    LogBufferStream& LogBufferStream::operator<<(::int8_t value)
    {
        buffer_.writeInt8(value);
        return *this;
    }

    LogBufferStream& LogBufferStream::operator<<(::uint8_t value)
    {
        buffer_.writeUint8(value);
        return *this;
    }

    LogBufferStream& LogBufferStream::operator<<(::int16_t value)
    {
        buffer_.writeInt16(value);
        return *this;
    }

    LogBufferStream& LogBufferStream::operator<<(::uint16_t value)
    {
        buffer_.writeUint16(value);
        return *this;
    }

    LogBufferStream& LogBufferStream::operator<<(::int32_t value)
    {
        buffer_.writeInt32(value);
        return *this;
    }

    LogBufferStream& LogBufferStream::operator<<(::uint32_t value)
    {
        buffer_.writeUint32(value);
        return *this;
    }

    LogBufferStream& LogBufferStream::operator<<(::int64_t value)
    {
        buffer_.writeInt64(value);
        return *this;
    }

    LogBufferStream& LogBufferStream::operator<<(::uint64_t value)
    {
        buffer_.writeUint64(value);
        return *this;
    }

    LogBufferStream& LogBufferStream::operator<<(const void* ptr)
    {
        buffer_.writePointer(ptr);
        return *this;
    }

    LogBufferStream& LogBufferStream::operator<<(float value)
    {
        buffer_.writeFloat(value);
        return *this;
    }

    LogBufferStream& LogBufferStream::operator<<(double value)
    {
        buffer_.writeDouble(value);
        return *this;
    }

    LogBufferStream& LogBufferStream::operator<<(const char* str)
    {
        buffer_.writeBytes(str, ::strlen(str));
        return *this;
    }

    LogBufferStream& LogBufferStream::operator<<(const ::std::string& str)
    {
        buffer_.writeBytes(str.c_str(), str.length());
        return *this;
    }

    LogBufferStream& LogBufferStream::operator<<(const std::string_view& str)
    {
        buffer_.writeBytes(str.data(), str.length());
        return *this;
    }

    LogBufferStream& LogBufferStream::operator<<(const LogBufferStream& stream)
    {
        buffer_.writeBytes(stream.buffer_.array(), stream.buffer_.length());
        return *this;
    }
} // namespace nets