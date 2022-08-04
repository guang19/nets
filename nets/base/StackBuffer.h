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
        using IntType = uint32_t;
    }

    template <IntType SIZE>
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

        inline IntType len() const
        {
            return writerIndex_;
        }

        inline IntType writerIndex() const
        {
            return writerIndex_;
        }

        inline IntType writableBytes() const
        {
            return capacity_ - writerIndex_;
        }

        void writeBytes(const char* data, IntType len);

        void writePointer(const void* ptr);

        template <typename Number>
        void writeInteger(Number n);

        template <typename Float>
        void writeFloat(Float f);

    private:
        char buffer_[SIZE] {0};
        // writer pointer
        IntType writerIndex_ {0};
        IntType capacity_ {SIZE};
    };

    template <IntType SIZE>
    void StackBuffer<SIZE>::writeBytes(const char* data, IntType len)
    {
        if (writableBytes() > len)
        {
            ::memcpy(buffer_ + writerIndex_, data, len);
            writerIndex_ += len;
        }
    }

    template <IntType SIZE>
    void StackBuffer<SIZE>::writePointer(const void* ptr)
    {
        if (writableBytes() > MaxNumLen)
        {
            writerIndex_ += utils::fromHex(buffer_ + writerIndex_, reinterpret_cast<uintptr_t>(ptr));
        }
    }

    template <IntType SIZE>
    template <typename Number>
    void StackBuffer<SIZE>::writeInteger(Number n)
    {
        if (writableBytes() > MaxNumLen)
        {
            writerIndex_ += utils::fromInt(buffer_ + writerIndex_, n);
        }
    }

    template <IntType SIZE>
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
