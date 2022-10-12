// MIT License
//
// Copyright (c) 2022 guang19
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

// @brief Log buffer

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