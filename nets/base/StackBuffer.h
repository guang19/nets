//
// Created by guang19
//

#ifndef NETS_STACK_BUFFER_H
#define NETS_STACK_BUFFER_H

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <limits>

#include "nets/base/CommonMacro.h"
#include "nets/base/Noncopyable.h"

namespace nets
{
    namespace
    {
        using SizeType = ::size_t;
        constexpr char kDigits[] = {"0123456789"};
        constexpr char kHexDigits[] = {"0123456789abcdef"};
        constexpr ::int32_t kMaximumNumberLimit = ::std::numeric_limits<::uint64_t>::digits10 + 1;
    } // namespace

    template <SizeType SIZE>
    class StackBuffer : Noncopyable
    {
    public:
        using IntType = ::int32_t;
        using LongType = ::int64_t;
        using StringType = ::std::string;

    public:
        StackBuffer() : writerIndex_(0), capacity_(SIZE)
        {
            MEMZERO(buffer_, SIZE);
        }

        ~StackBuffer() = default;

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
            return capacity_ - writerIndex_;
        }

        void writeString(const StringType& str)
        {
            writeBytes(str.data(), str.length());
        }

        void writeBytes(const char* data, SizeType length)
        {
            if (writableBytes() > length)
            {
                ::memcpy(buffer_ + writerIndex_, data, length);
                writerIndex_ += length;
            }
        }

        void writeByte(char value)
        {
            writeBytes(&value, 1);
        }

        void writeInt8(::int8_t value)
        {
            writeInt32(static_cast<::int32_t>(value));
        }

        void writeUint8(::uint8_t value)
        {
            writeUint32(static_cast<::uint32_t>(value));
        }

        void writeInt16(::int16_t value)
        {
            writeInt32(static_cast<::int32_t>(value));
        }

        void writeUint16(::uint16_t value)
        {
            writeUint32(static_cast<::uint32_t>(value));
        }

        void writeInt32(::int32_t value)
        {
            writeInteger(value);
        }

        void writeUint32(::uint32_t value)
        {
            writeInteger(value);
        }

        void writeInt64(::int64_t value)
        {
            writeInteger(value);
        }

        void writeUint64(::uint64_t value)
        {
            writeInteger(value);
        }

        template <typename IntType>
        void writeInteger(IntType value)
        {
            if (writableBytes() > kMaximumNumberLimit)
            {
                char* buffer = buffer_ + writerIndex_;
                char* tmp = buffer;
                do
                {
                    auto lastIndex = static_cast<::int32_t>(value % 10);
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

        void writeFloat(float value)
        {
            writeDouble(static_cast<double>(value));
        }

        void writeDouble(double value)
        {
            writeString(::std::to_string(value));
        }

        void writePointer(const void* ptr)
        {
            if (writableBytes() > kMaximumNumberLimit)
            {
                auto value = reinterpret_cast<::uintptr_t>(ptr);
                char* buffer = buffer_ + writerIndex_;
                char* tmp = buffer;
                tmp[0] = '0';
                tmp[1] = 'x';
                tmp += 2;
                do
                {
                    auto lastIndex = static_cast<::int32_t>(value % 16);
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
        SizeType capacity_;
    };
} // namespace nets

#endif // NETS_STACK_BUFFER_H