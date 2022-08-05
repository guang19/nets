//
// Created by guang19 on 2022/5/7.
//

#include "nets/net/core/ByteBuffer.h"

#include <cassert>
#include <stdexcept>

#include "nets/base/CommonMacro.h"
#include "nets/base/exception/OutOfMemoryException.h"
#include "nets/net/core/SocketChannel.h"

namespace nets::net
{
    namespace
    {
        constexpr ByteBuffer::IntType DefaultInitialCapacity = 1024;
        constexpr ByteBuffer::IntType MaxCapacity = INT32_MAX;

        constexpr ByteBuffer::IntType BooleanBytes = sizeof(bool);
        constexpr ByteBuffer::IntType CharBytes = sizeof(char);
        constexpr ByteBuffer::IntType Int8Bytes = sizeof(int8_t);
        constexpr ByteBuffer::IntType Int16Bytes = sizeof(int16_t);
        constexpr ByteBuffer::IntType Int32Bytes = sizeof(int32_t);
        constexpr ByteBuffer::IntType Int64Bytes = sizeof(int64_t);
        constexpr ByteBuffer::IntType FloatBytes = sizeof(float);
        constexpr ByteBuffer::IntType DoubleBytes = sizeof(double);
    } // namespace

    ByteBuffer::ByteBuffer() : ByteBuffer(DefaultInitialCapacity) {}

    ByteBuffer::ByteBuffer(IntType capacity)
        : buffer_(::std::make_unique<char[]>(capacity)), readerIndex_(0), writerIndex_(0), capacity_(capacity)
    {
        MEMZERO(&buffer_[0], capacity_);
    }

    ByteBuffer::ByteBuffer(const ByteBuffer& other)
    {
        readerIndex_ = other.readerIndex_;
        writerIndex_ = other.writerIndex_;
        capacity_ = other.capacity_;
        buffer_.reset(new char[capacity_]);
        MEMZERO(&buffer_[0], capacity_);
        ::memcpy(&buffer_[0], &other.buffer_[0], capacity_);
    }

    ByteBuffer::ByteBuffer(ByteBuffer&& other) noexcept
    {
        readerIndex_ = other.readerIndex_;
        writerIndex_ = other.writerIndex_;
        capacity_ = other.capacity_;
        buffer_ = ::std::move(other.buffer_);
        assert(other.buffer_ == nullptr);
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
            MEMZERO(&buffer_[0], capacity_);
            ::memcpy(&buffer_[0], &other.buffer_[0], capacity_);
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
            assert(other.buffer_ == nullptr);
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

    void ByteBuffer::writeBoolean(bool value)
    {
        writeBytes(&value, BooleanBytes);
    }

    void ByteBuffer::writeByte(char value)
    {
        writeBytes(&value, CharBytes);
    }

    void ByteBuffer::writeBytes(const void* data, IntType len)
    {
        writeBytes(reinterpret_cast<const char*>(data), len);
    }

    void ByteBuffer::writeBytes(const char* data, IntType len)
    {
        ensureWritable(len);
        ::memcpy(&buffer_[writerIndex_], data, len);
        writerIndex_ += len;
    }

    SSizeType ByteBuffer::writeBytes(SocketChannel& channel)
    {
        SSizeType bytes = 0;
        bytes = socket::read(channel.fd(), &buffer_[writerIndex_], writableBytes());
        if (bytes > 0)
        {
            writerIndex_ += bytes;
        }
        return bytes;
    }

    void ByteBuffer::writeInt8(int8_t value)
    {
        writeBytes(&value, Int8Bytes);
    }

    void ByteBuffer::writeInt16(int16_t value)
    {
        int16_t tmp = htobe16(value);
        writeBytes(&tmp, Int16Bytes);
    }

    void ByteBuffer::writeInt32(int32_t value)
    {
        int32_t tmp = htobe32(value);
        writeBytes(&tmp, Int32Bytes);
    }

    void ByteBuffer::writeInt64(int64_t value)
    {
        int64_t tmp = htobe64(value);
        writeBytes(&tmp, Int64Bytes);
    }

    void ByteBuffer::writeFloat(float value)
    {
        writeBytes(&value, FloatBytes);
    }

    void ByteBuffer::writeDouble(double value)
    {
        writeBytes(&value, DoubleBytes);
    }

    bool ByteBuffer::readBoolean()
    {
        checkReadableBytes(BooleanBytes);
        bool val = false;
        ::memcpy(&val, &buffer_[readerIndex_], BooleanBytes);
        adjustReaderIndex(BooleanBytes);
        return val;
    }

    char ByteBuffer::readByte()
    {
        checkReadableBytes(CharBytes);
        char c = 0;
        ::memcpy(&c, &buffer_[readerIndex_], CharBytes);
        adjustReaderIndex(CharBytes);
        return c;
    }

    ByteBuffer::StringType ByteBuffer::readBytes(IntType len)
    {
        checkReadableBytes(len);
        StringType s(&buffer_[readerIndex_], len);
        adjustReaderIndex(len);
        return s;
    }

    int8_t ByteBuffer::readInt8()
    {
        checkReadableBytes(Int8Bytes);
        int8_t val = 0;
        ::memcpy(&val, &buffer_[readerIndex_], Int8Bytes);
        adjustReaderIndex(Int8Bytes);
        return val;
    }

    int16_t ByteBuffer::readInt16()
    {
        checkReadableBytes(Int16Bytes);
        int16_t val = 0;
        ::memcpy(&val, &buffer_[readerIndex_], Int16Bytes);
        adjustReaderIndex(Int16Bytes);
        return be16toh(val);
    }

    int32_t ByteBuffer::readInt32()
    {
        checkReadableBytes(Int32Bytes);
        int32_t val = 0;
        ::memcpy(&val, &buffer_[readerIndex_], Int32Bytes);
        adjustReaderIndex(Int32Bytes);
        return be32toh(val);
    }

    int64_t ByteBuffer::readInt64()
    {
        checkReadableBytes(Int64Bytes);
        int64_t val = 0;
        ::memcpy(&val, &buffer_[readerIndex_], Int64Bytes);
        adjustReaderIndex(Int64Bytes);
        return be64toh(val);
    }

    float ByteBuffer::readFloat()
    {
        checkReadableBytes(FloatBytes);
        float val = 0;
        ::memcpy(&val, &buffer_[readerIndex_], FloatBytes);
        adjustReaderIndex(FloatBytes);
        return *(float*) &val;
    }

    double ByteBuffer::readDouble()
    {
        checkReadableBytes(DoubleBytes);
        double val;
        ::memcpy(&val, &buffer_[readerIndex_], DoubleBytes);
        adjustReaderIndex(DoubleBytes);
        return *(double*) &val;
    }

    void ByteBuffer::ensureWritable(IntType writeLen)
    {
        if (writableBytes() < writeLen)
        {
            IntType newCapacity = capacity_;
            if (capacity_ != 0)
            {
                if (writableBytes() + discardReadBytes() < writeLen)
                {
                    IntType targetCapacity = writerIndex_ + writeLen;
                    newCapacity = calculateNewCapacity(targetCapacity);
                }
                if (newCapacity > MaxCapacity)
                {
                    THROW_FMT(nets::base::OutOfMemoryException, "ByteBuffer newCapacity %lu exceeds the MaxCapacity",
                              newCapacity);
                }
            }
            else
            {
                newCapacity = DefaultInitialCapacity;
            }
            adjustCapacity(newCapacity);
        }
    }

    ByteBuffer::IntType ByteBuffer::calculateNewCapacity(IntType targetCapacity) const
    {
        if (targetCapacity == 0)
        {
            return 0;
        }
        IntType newCapacity = 1;
        while (newCapacity < targetCapacity)
        {
            newCapacity <<= 1;
        }
        return newCapacity;
    }

    void ByteBuffer::adjustCapacity(IntType newCapacity)
    {
        IntType contentBytes = readableBytes();
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

    void ByteBuffer::checkReadableBytes(IntType bytes) const
    {
        if (readableBytes() < bytes)
        {
            THROW_FMT(nets::base::OutOfMemoryException, "ByteBuffer readableBytes less than bytes %u", bytes);
        }
    }

    void ByteBuffer::adjustReaderIndex(IntType bytes)
    {
        if (readableBytes() > bytes)
        {
            readerIndex_ += bytes;
        }
        else
        {
            clear();
        }
    }
} // namespace nets::net