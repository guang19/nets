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

// @brief Stack based buffer

#ifndef NETS_STACK_BUFFER_H
#define NETS_STACK_BUFFER_H

#include <algorithm>
#include <cmath>
#include <limits>

#include "nets/base/Common.h"
#include "nets/base/Copyable.h"

namespace nets
{
    namespace
    {
        constexpr char kDigits[] = {"0123456789"};
        constexpr char kHexDigits[] = {"0123456789abcdef"};
        constexpr Int32Type kMaximumNumberLimit = ::std::numeric_limits<Uint64Type>::digits10 + 1;
    } // namespace

    template <SizeType SIZE>
    class StackBuffer : public Copyable
    {
    public:
        StackBuffer() : writerIndex_(0)
        {
            MEMZERO(buffer_, SIZE);
        }

        ~StackBuffer() = default;

        StackBuffer(const StackBuffer& other)
        {
            MEMZERO(buffer_, SIZE);
            ::memcpy(&buffer_[0], &other.buffer_[0], other.writerIndex_);
            writerIndex_ = other.writerIndex_;
        }

        StackBuffer(StackBuffer&& other) noexcept
        {
            MEMZERO(buffer_, SIZE);
            ::memcpy(&buffer_[0], &other.buffer_[0], other.writerIndex_);
            writerIndex_ = other.writerIndex_;
            MEMZERO(other.buffer_, other.writerIndex_);
            other.writerIndex_ = 0;
        }

        StackBuffer& operator=(const StackBuffer& other)
        {
            if (this != &other)
            {
                MEMZERO(buffer_, writerIndex_);
                ::memcpy(&buffer_[0], &other.buffer_[0], other.writerIndex_);
                writerIndex_ = other.writerIndex_;
            }
        }

        StackBuffer& operator=(StackBuffer&& other) noexcept
        {
            if (this != &other)
            {
                MEMZERO(buffer_, writerIndex_);
                ::memcpy(&buffer_[0], &other.buffer_[0], other.writerIndex_);
                writerIndex_ = other.writerIndex_;
                MEMZERO(other.buffer_, other.writerIndex_);
                other.writerIndex_ = 0;
            }
        }

        void swap(StackBuffer& other) noexcept
        {
            // ::std::swap(buffer, other.buffer), but for SIZE
            SizeType n = writerIndex_ < other.writerIndex_ ? other.writerIndex_ : writerIndex_;
            for (SizeType i = 0; i < n; ++i)
            {
                ::std::swap(buffer_[i], other.buffer_[i]);
            }
            ::std::swap(writerIndex_, other.writerIndex_);
        }

    public:
        inline const char* array() const
        {
            return buffer_;
        }

        inline SizeType length() const
        {
            return writerIndex_;
        }

        inline SizeType writerIndex() const
        {
            return writerIndex_;
        }

        inline SizeType writableBytes() const
        {
            return SIZE - writerIndex_;
        }

        inline void writeString(const StringType& str)
        {
            writeBytes(str.data(), str.length());
        }

        inline void writeBytes(const char* data)
        {
            writeBytes(data, ::strlen(data));
        }

        inline void writeBytes(const StackBuffer<SIZE>& buffer)
        {
            writeBytes(&buffer.buffer_[0], buffer.writerIndex_);
        }

        inline void writeBytes(const char* data, SizeType length)
        {
            if (writableBytes() > length)
            {
                ::memcpy(buffer_ + writerIndex_, data, length);
                writerIndex_ += length;
            }
        }

        inline void writeByte(char value)
        {
            writeBytes(&value, 1);
        }

        inline void writeInt8(Int8Type value)
        {
            writeInt32(static_cast<Uint32Type>(value));
        }

        inline void writeUint8(Uint8Type value)
        {
            writeUint32(static_cast<Uint32Type>(value));
        }

        inline void writeInt16(Int16Type value)
        {
            writeInt32(static_cast<Int32Type>(value));
        }

        inline void writeUint16(Uint16Type value)
        {
            writeUint32(static_cast<Uint32Type>(value));
        }

        inline void writeInt32(Int32Type value)
        {
            writeInteger(value);
        }

        inline void writeUint32(Uint32Type value)
        {
            writeInteger(value);
        }

        inline void writeInt64(Int64Type value)
        {
            writeInteger(value);
        }

        inline void writeUint64(Uint64Type value)
        {
            writeInteger(value);
        }

        template <typename IntType>
        inline void writeInteger(IntType value)
        {
            if (writableBytes() > kMaximumNumberLimit)
            {
                char* buffer = buffer_ + writerIndex_;
                char* tmp = buffer;
                do
                {
                    auto lastIndex = static_cast<Int32Type>(value % 10);
                    *tmp = kDigits[lastIndex];
                    ++tmp;
                    value /= 10;
                } while (value > 0);
                if (value < 0)
                {
                    *tmp = '-';
                    ++tmp;
                }
                ::std::reverse(buffer, tmp);
                writerIndex_ += tmp - buffer;
            }
        }

        inline void writeFloat(float value)
        {
            writeDouble(static_cast<double>(value));
        }

        inline void writeDouble(double value)
        {
            writeString(::std::to_string(value));
        }

        inline void writePointer(const void* ptr)
        {
            if (writableBytes() > kMaximumNumberLimit)
            {
                auto value = reinterpret_cast<UintPtrType>(ptr);
                char* buffer = buffer_ + writerIndex_;
                char* tmp = buffer;
                tmp[0] = '0';
                tmp[1] = 'x';
                tmp += 2;
                do
                {
                    auto lastIndex = static_cast<Int32Type>(value % 16);
                    *tmp = kHexDigits[lastIndex];
                    ++tmp;
                    value /= 16;
                } while (value > 0);
                ::std::reverse(buffer + 2, tmp);
                writerIndex_ += tmp - buffer;
            }
        }

    private:
        char buffer_[SIZE] {0};
        // writer pointer
        SizeType writerIndex_;
    };
} // namespace nets

#endif // NETS_STACK_BUFFER_H