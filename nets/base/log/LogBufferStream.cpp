//
// Created by YangGuang on 2022/4/3.
//

#include "nets/base/log/LogBufferStream.h"

namespace nets::base
{
    LogBufferStream& LogBufferStream::operator<<(char c)
    {
        buffer_.writeBytes(&c, 1);
        return *this;
    }

    LogBufferStream& LogBufferStream::operator<<(int8_t n)
    {
        buffer_.writeInteger(n);
        return *this;
    }

    LogBufferStream& LogBufferStream::operator<<(uint8_t n)
    {
        buffer_.writeInteger(n);
        return *this;
    }

    LogBufferStream& LogBufferStream::operator<<(int16_t n)
    {
        buffer_.writeInteger(n);
        return *this;
    }

    LogBufferStream& LogBufferStream::operator<<(uint16_t n)
    {
        buffer_.writeInteger(n);
        return *this;
    }

    LogBufferStream& LogBufferStream::operator<<(int32_t n)
    {
        buffer_.writeInteger(n);
        return *this;
    }

    LogBufferStream& LogBufferStream::operator<<(uint32_t n)
    {
        buffer_.writeInteger(n);
        return *this;
    }

    LogBufferStream& LogBufferStream::operator<<(int64_t n)
    {
        buffer_.writeInteger(n);
        return *this;
    }

    LogBufferStream& LogBufferStream::operator<<(uint64_t n)
    {
        buffer_.writeInteger(n);
        return *this;
    }

    LogBufferStream& LogBufferStream::operator<<(const void* ptr)
    {
        buffer_.writePointer(ptr);
        return *this;
    }

    LogBufferStream& LogBufferStream::operator<<(float f)
    {
        buffer_.writeFloat(f);
        return *this;
    }

    LogBufferStream& LogBufferStream::operator<<(double f)
    {
        buffer_.writeFloat(f);
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
} // namespace nets::base