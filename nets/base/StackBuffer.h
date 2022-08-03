//
// Created by guang19 on 2022/6/9.
//

#ifndef NETS_BASE_STACK_BUFFER_H
#define NETS_BASE_STACK_BUFFER_H

#include <cstdint>

#include "nets/base/CommonMacro.h"
#include "nets/base/Noncopyable.h"
#include "nets/base/StringUtils.h"

namespace nets::base
{
    namespace
    {
        using SizeType = uint32_t;
    }

    template <SizeType SIZE>
    class StackBuffer : Noncopyable
    {
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

        inline SizeType len() const
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

        void writeBytes(const char* data, SizeType len);

        void writePointer(const void* ptr);

        template <typename Number>
        void writeInteger(Number n);

        template <typename Float>
        void writeFloat(Float f);

    private:
        char buffer_[SIZE] {0};
        // writer pointer
        SizeType writerIndex_ {0};
        SizeType capacity_ {SIZE};
    };

    template <SizeType SIZE>
    void StackBuffer<SIZE>::writeBytes(const char* data, SizeType len)
    {
        if (writableBytes() > len)
        {
            ::memcpy(buffer_ + writerIndex_, data, len);
            writerIndex_ += len;
        }
    }

    template <SizeType SIZE>
    void StackBuffer<SIZE>::writePointer(const void* ptr)
    {
        if (writableBytes() > MaxNumLen)
        {
            writerIndex_ += utils::fromHex(buffer_ + writerIndex_, reinterpret_cast<uintptr_t>(ptr));
        }
    }

    template <SizeType SIZE>
    template <typename Number>
    void StackBuffer<SIZE>::writeInteger(Number n)
    {
        if (writableBytes() > MaxNumLen)
        {
            writerIndex_ += utils::fromInt(buffer_ + writerIndex_, n);
        }
    }

    template <SizeType SIZE>
    template <typename Float>
    void StackBuffer<SIZE>::writeFloat(Float f)
    {
        if (writableBytes() > MaxFloatLen)
        {
            writerIndex_ += utils::fromFloat(buffer_ + writerIndex_, f);
        }
    }
} // namespace nets::base

#endif // NETS_BASE_STACK_BUFFER_H
