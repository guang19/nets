//
// Created by guang19
//

#include "nets/net/core/ByteBuffer.h"

#include <cassert>
#include <stdexcept>

#include "nets/base/CommonMacro.h"
#include "nets/base/exception/OutOfMemoryException.h"
#include "nets/net/channel/SocketChannel.h"

namespace nets::net
{
    namespace
    {
        constexpr ByteBuffer::SizeType DefaultInitialCapacity = 1024;
        constexpr ByteBuffer::SizeType MaxCapacity = INT32_MAX - 1;

        constexpr ByteBuffer::SizeType BooleanBytes = sizeof(bool);
        constexpr ByteBuffer::SizeType CharBytes = sizeof(char);
        constexpr ByteBuffer::SizeType Int8Bytes = sizeof(::int8_t);
        constexpr ByteBuffer::SizeType Int16Bytes = sizeof(::int16_t);
        constexpr ByteBuffer::SizeType Int32Bytes = sizeof(::int32_t);
        constexpr ByteBuffer::SizeType Int64Bytes = sizeof(::int64_t);
        constexpr ByteBuffer::SizeType FloatBytes = sizeof(float);
        constexpr ByteBuffer::SizeType DoubleBytes = sizeof(double);
    } // namespace

    ByteBuffer::ByteBuffer() : ByteBuffer(DefaultInitialCapacity) {}

    ByteBuffer::ByteBuffer(SizeType capacity)
        : buffer_(::std::make_unique<char[]>(capacity)), readerIndex_(0), writerIndex_(0), capacity_(capacity)
    {
        MEMZERO(&buffer_[0], capacity_);
    }

    ByteBuffer::ByteBuffer(const void* data, SizeType length)
        : buffer_(nullptr), readerIndex_(0), writerIndex_(0), capacity_(0)
    {
        writeBytes(data, length);
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
        : buffer_(::std::move(other.buffer_)), readerIndex_(other.readerIndex_), writerIndex_(other.writerIndex_),
          capacity_(other.capacity_)
    {
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

    void ByteBuffer::writeBytes(const void* data, SizeType length)
    {
        writeBytes(reinterpret_cast<const char*>(data), length);
    }

    void ByteBuffer::writeBytes(const char* data, SizeType length)
    {
        ensureWritable(length);
        ::memcpy(&buffer_[writerIndex_], data, length);
        writerIndex_ += length;
    }

    SSizeType ByteBuffer::writeBytes(const SocketChannel& channel, SizeType length)
    {
        ensureWritable(length);
        SSizeType bytes = 0;
        bytes = socket::read(channel.fd(), &buffer_[writerIndex_], length);
        if (bytes > 0)
        {
            writerIndex_ += bytes;
        }
        return bytes;
    }

    void ByteBuffer::writeInt8(::int8_t value)
    {
        writeBytes(&value, Int8Bytes);
    }

    void ByteBuffer::writeInt16(::int16_t value)
    {
        ::int16_t tmp = htobe16(value);
        writeBytes(&tmp, Int16Bytes);
    }

    void ByteBuffer::writeInt32(::int32_t value)
    {
        ::int32_t tmp = htobe32(value);
        writeBytes(&tmp, Int32Bytes);
    }

    void ByteBuffer::writeInt64(::int64_t value)
    {
        ::int64_t tmp = htobe64(value);
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

    ByteBuffer::StringType ByteBuffer::readBytes(SizeType length)
    {
        checkReadableBytes(length);
        StringType s(&buffer_[readerIndex_], length);
        adjustReaderIndex(length);
        return s;
    }

    ::int8_t ByteBuffer::readInt8()
    {
        checkReadableBytes(Int8Bytes);
        ::int8_t val = 0;
        ::memcpy(&val, &buffer_[readerIndex_], Int8Bytes);
        adjustReaderIndex(Int8Bytes);
        return val;
    }

    ::int16_t ByteBuffer::readInt16()
    {
        checkReadableBytes(Int16Bytes);
        ::int16_t val = 0;
        ::memcpy(&val, &buffer_[readerIndex_], Int16Bytes);
        adjustReaderIndex(Int16Bytes);
        return be16toh(val);
    }

    ::int32_t ByteBuffer::readInt32()
    {
        checkReadableBytes(Int32Bytes);
        ::int32_t val = 0;
        ::memcpy(&val, &buffer_[readerIndex_], Int32Bytes);
        adjustReaderIndex(Int32Bytes);
        return be32toh(val);
    }

    ::int64_t ByteBuffer::readInt64()
    {
        checkReadableBytes(Int64Bytes);
        ::int64_t val = 0L;
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
        double val = 0;
        ::memcpy(&val, &buffer_[readerIndex_], DoubleBytes);
        adjustReaderIndex(DoubleBytes);
        return *(double*) &val;
    }

    ByteBuffer::StringType ByteBuffer::toString() const
    {
        return {&buffer_[readerIndex_], readableBytes()};
    }

    void ByteBuffer::ensureWritable(SizeType writeLen)
    {
        if (writableBytes() < writeLen)
        {
            SizeType newCapacity = capacity_;
            if (capacity_ != 0)
            {
                if (writableBytes() + discardReadBytes() < writeLen)
                {
                    SizeType targetCapacity = writerIndex_ + writeLen;
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
                newCapacity = calculateNewCapacity(writeLen);
            }
            adjustCapacity(newCapacity);
        }
    }

    ByteBuffer::SizeType ByteBuffer::calculateNewCapacity(SizeType targetCapacity) const
    {
        // ensure newCapacity is a power of 2
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
            MEMZERO(&newBuffer[0], newCapacity);
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

    void ByteBuffer::checkReadableBytes(SizeType bytes) const
    {
        if (readableBytes() < bytes)
        {
            THROW_FMT(nets::base::OutOfMemoryException, "ByteBuffer readableBytes less than bytes %u", bytes);
        }
    }

    void ByteBuffer::adjustReaderIndex(SizeType bytes)
    {
        if (readableBytes() > bytes)
        {
            readerIndex_ += bytes;
        }
        else
        {
            // readerIndex >= writerIndex
            clear();
        }
    }
} // namespace nets::net