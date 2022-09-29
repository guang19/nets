//
// Created by guang19
//

#include "nets/net/core/ByteBuffer.h"

#include <cassert>
#include <stdexcept>

#include "nets/base/CommonMacro.h"
#include "nets/base/exception/OutOfMemoryException.h"
#include "nets/net/channel/DatagramChannel.h"
#include "nets/net/channel/SocketChannel.h"

namespace nets::net
{
    namespace
    {
        constexpr ByteBuffer::SizeType gDefaultInitialCapacity = 1024;
        constexpr ByteBuffer::SizeType gMaxCapacity = INT32_MAX - 1;

        constexpr ByteBuffer::SizeType gBooleanSize = sizeof(bool);
        constexpr ByteBuffer::SizeType gCharSize = sizeof(char);
        constexpr ByteBuffer::SizeType gInt8Size = sizeof(::int8_t);
        constexpr ByteBuffer::SizeType gInt16Size = sizeof(::int16_t);
        constexpr ByteBuffer::SizeType gInt32Size = sizeof(::int32_t);
        constexpr ByteBuffer::SizeType gInt64Size = sizeof(::int64_t);
        constexpr ByteBuffer::SizeType gFloatSize = sizeof(float);
        constexpr ByteBuffer::SizeType gDoubleSize = sizeof(double);
    } // namespace

    ByteBuffer::ByteBuffer() : ByteBuffer(gDefaultInitialCapacity) {}

    ByteBuffer::ByteBuffer(SizeType capacity)
        : buffer_(::std::make_unique<char[]>(capacity)), readerIndex_(0), writerIndex_(0), capacity_(capacity)
    {
        MEMZERO(&buffer_[0], capacity_);
    }

    ByteBuffer::ByteBuffer(const char* data) : ByteBuffer(data, ::strlen(data)) {}

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
        writeBytes(&value, gBooleanSize);
    }

    void ByteBuffer::writeByte(char value)
    {
        writeBytes(&value, gCharSize);
    }

    void ByteBuffer::writeBytes(const char* data)
    {
        writeBytes(data, ::strlen(data));
    }

    void ByteBuffer::writeBytes(const void* data, SizeType length)
    {
        writeBytes(static_cast<const char*>(data), length);
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
        SSizeType bytes = socket::read(channel.fd(), &buffer_[writerIndex_], length);
        if (bytes > 0)
        {
            writerIndex_ += bytes;
        }
        return bytes;
    }

    SSizeType ByteBuffer::writeBytes(const DatagramChannel& channel, SizeType length, InetSockAddress& srcAddr)
    {
        ensureWritable(length);
        SSizeType bytes = socket::recvFrom(channel.fd(), &buffer_[writerIndex_], length, srcAddr);
        if (bytes > 0)
        {
            writerIndex_ += bytes;
        }
        return bytes;
    }

    void ByteBuffer::writeInt8(::int8_t value)
    {
        writeBytes(&value, gInt8Size);
    }

    void ByteBuffer::writeInt16(::int16_t value)
    {
        ::int16_t tmp = htobe16(value);
        writeBytes(&tmp, gInt16Size);
    }

    void ByteBuffer::writeInt32(::int32_t value)
    {
        ::int32_t tmp = htobe32(value);
        writeBytes(&tmp, gInt32Size);
    }

    void ByteBuffer::writeInt64(::int64_t value)
    {
        ::int64_t tmp = htobe64(value);
        writeBytes(&tmp, gInt64Size);
    }

    void ByteBuffer::writeFloat(float value)
    {
        writeBytes(&value, gFloatSize);
    }

    void ByteBuffer::writeDouble(double value)
    {
        writeBytes(&value, gDoubleSize);
    }

    bool ByteBuffer::readBoolean()
    {
        checkReadableBytes(gBooleanSize);
        bool val = false;
        ::memcpy(&val, &buffer_[readerIndex_], gBooleanSize);
        adjustReaderIndex(gBooleanSize);
        return val;
    }

    char ByteBuffer::readByte()
    {
        checkReadableBytes(gCharSize);
        char c = 0;
        ::memcpy(&c, &buffer_[readerIndex_], gCharSize);
        adjustReaderIndex(gCharSize);
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
        checkReadableBytes(gInt8Size);
        ::int8_t val = 0;
        ::memcpy(&val, &buffer_[readerIndex_], gInt8Size);
        adjustReaderIndex(gInt8Size);
        return val;
    }

    ::int16_t ByteBuffer::readInt16()
    {
        checkReadableBytes(gInt16Size);
        ::int16_t val = 0;
        ::memcpy(&val, &buffer_[readerIndex_], gInt16Size);
        adjustReaderIndex(gInt16Size);
        return be16toh(val);
    }

    ::int32_t ByteBuffer::readInt32()
    {
        checkReadableBytes(gInt32Size);
        ::int32_t val = 0;
        ::memcpy(&val, &buffer_[readerIndex_], gInt32Size);
        adjustReaderIndex(gInt32Size);
        return be32toh(val);
    }

    ::int64_t ByteBuffer::readInt64()
    {
        checkReadableBytes(gInt64Size);
        ::int64_t val = 0L;
        ::memcpy(&val, &buffer_[readerIndex_], gInt64Size);
        adjustReaderIndex(gInt64Size);
        return be64toh(val);
    }

    float ByteBuffer::readFloat()
    {
        checkReadableBytes(gFloatSize);
        float val = 0;
        ::memcpy(&val, &buffer_[readerIndex_], gFloatSize);
        adjustReaderIndex(gFloatSize);
        return *(float*) &val;
    }

    double ByteBuffer::readDouble()
    {
        checkReadableBytes(gDoubleSize);
        double val = 0;
        ::memcpy(&val, &buffer_[readerIndex_], gDoubleSize);
        adjustReaderIndex(gDoubleSize);
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
                if (newCapacity > gMaxCapacity)
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