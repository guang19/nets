//
// Created by guang19 on 2022/5/7.
//

#include "nets/net/core/ByteBuffer.h"

#include <stdexcept>

#include "nets/base/CommonMacro.h"

namespace nets::net
{
    namespace
    {
        constexpr ByteBuffer::SizeType DefaultInitialCapacity = 1024;
        constexpr ByteBuffer::SizeType MaxCapacity = INT32_MAX;
    }

    ByteBuffer::ByteBuffer() : ByteBuffer(DefaultInitialCapacity)
    {}

    ByteBuffer::ByteBuffer(SizeType capacity)
        : buffer_(::std::make_unique<char[]>(capacity)), readerIndex_(0), writerIndex_(0), capacity_(capacity)
    {
        MEMZERO(buffer_.get(), capacity_);
    }

    ByteBuffer::ByteBuffer(const ByteBuffer& other)
    {
        readerIndex_ = other.readerIndex_;
        writerIndex_ = other.writerIndex_;
        capacity_ = other.capacity_;
        buffer_.reset(new char[capacity_]);
        MEMZERO(buffer_.get(), capacity_);
        ::memcpy(buffer_.get(), other.buffer_.get(), capacity_);
    }

    ByteBuffer::ByteBuffer(ByteBuffer&& other) noexcept
    {
        readerIndex_ = other.readerIndex_;
        writerIndex_ = other.writerIndex_;
        capacity_ = other.capacity_;
        buffer_ = ::std::move(other.buffer_);
        other.readerIndex_ = 0;
        other.writerIndex_ = 0;
        other.capacity_ = 0;
    }

    ByteBuffer& ByteBuffer::operator=(const ByteBuffer& other)
    {
        if (this != &other)
        {
            readerIndex_ = other.readerIndex_;
            writerIndex_ = other.writerIndex_;
            capacity_ = other.capacity_;
            buffer_.reset(new char[capacity_]);
            MEMZERO(buffer_.get(), capacity_);
            ::memcpy(buffer_.get(), other.buffer_.get(), capacity_);
        }
        return *this;
    }

    ByteBuffer& ByteBuffer::operator=(ByteBuffer&& other) noexcept
    {
        if (this != &other)
        {
            readerIndex_ = other.readerIndex_;
            writerIndex_ = other.writerIndex_;
            capacity_ = other.capacity_;
            buffer_ = std::move(other.buffer_);
            other.readerIndex_ = 0;
            other.writerIndex_ = 0;
            other.capacity_ = 0;
        }
        return *this;
    }

    void ByteBuffer::swap(ByteBuffer&& other)
    {
        ::std::swap(buffer_, other.buffer_);
        ::std::swap(readerIndex_, other.readerIndex_);
        ::std::swap(writerIndex_, other.writerIndex_);
        ::std::swap(capacity_, other.capacity_);
    }

    void ByteBuffer::writeBytes(const char* data, SizeType len)
    {
        ensureWritable(len);
        ::memcpy(&buffer_[writerIndex_], data, len);
        writerIndex_ += len;
    }

    void ByteBuffer::ensureWritable(SizeType writeLen)
    {
        if (writableBytes() < writeLen)
        {
            SizeType oldCapacity = capacity_;
            SizeType newCapacity = oldCapacity;
            if (oldCapacity != 0)
            {
                if (writableBytes() + discardBytes() < writeLen)
                {
                    SizeType targetCapacity = writerIndex_ + writeLen;
                    newCapacity = calculateNewCapacity(targetCapacity);
                }
                if (newCapacity > MaxCapacity)
                {
                    THROW_FMT(::std::length_error, "ByteBuffer newCapacity exceeds the MaxCapacity");
                }
            }
            else
            {
                newCapacity = DefaultInitialCapacity;
            }
            adjustCapacity(newCapacity);
        }
    }

    ByteBuffer::SizeType ByteBuffer::calculateNewCapacity(SizeType targetCapacity)
    {
        if (targetCapacity == 0)
        {
            return 0;
        }
        SizeType newCapacity = 1;
        while (newCapacity < targetCapacity)
        {
            newCapacity <<= 1;
        }
        return newCapacity;
    }

    void ByteBuffer::adjustCapacity(SizeType newCapacity)
    {
        SizeType contentBytes = readableBytes();
        // expansion
        if (newCapacity != capacity_)
        {
            CharArrayPtr newBuffer = ::std::make_unique<char[]>(newCapacity);
            if (contentBytes > 0)
            {
                ::memcpy(&newBuffer[0], &buffer_[readerIndex_], contentBytes);
            }
            buffer_.swap(newBuffer);
            capacity_ = newCapacity;
        }
        else
        {
            // replace discardBytes with readableBytes
            ::memmove(&buffer_[0], &buffer_[readerIndex_], contentBytes);
        }
        readerIndex_ = 0;
        writerIndex_ = contentBytes;
    }
} // namespace nets::net