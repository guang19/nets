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

// @brief The container for transferring binary data over the network

#include "nets/net/core/ByteBuffer.h"

#include <cassert>
#include <stdexcept>

#include "nets/base/Common.h"
#include "nets/base/exception/IndexOutOfBoundsException.h"
#include "nets/base/exception/OutOfMemoryException.h"
#include "nets/net/channel/DatagramChannel.h"
#include "nets/net/channel/SocketChannel.h"

namespace nets
{
    namespace
    {
        constexpr SizeType kDefaultInitialCapacity = 1024;
        constexpr SizeType kMaxCapacity = INT32_MAX - 1;

        constexpr SizeType kBooleanSize = sizeof(bool);
        constexpr SizeType kCharSize = sizeof(char);
        constexpr SizeType kInt8Size = sizeof(Int8Type);
        constexpr SizeType kInt16Size = sizeof(Int16Type);
        constexpr SizeType kInt32Size = sizeof(Int32Type);
        constexpr SizeType kInt64Size = sizeof(Int64Type);
        constexpr SizeType kFloatSize = sizeof(float);
        constexpr SizeType kDoubleSize = sizeof(double);
    } // namespace

    ByteBuffer::ByteBuffer() : ByteBuffer(kDefaultInitialCapacity) {}

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
        buffer_ = ::std::make_unique<char[]>(other.capacity_);
        MEMZERO(&buffer_[0], other.capacity_);
        ::memcpy(&buffer_[0], &other.buffer_[0], other.capacity_);
        readerIndex_ = other.readerIndex_;
        writerIndex_ = other.writerIndex_;
        capacity_ = other.capacity_;
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
            buffer_ = ::std::make_unique<char[]>(other.capacity_);
            MEMZERO(&buffer_[0], other.capacity_);
            ::memcpy(&buffer_[0], &other.buffer_[0], other.capacity_);
            readerIndex_ = other.readerIndex_;
            writerIndex_ = other.writerIndex_;
            capacity_ = other.capacity_;
        }
        return *this;
    }

    ByteBuffer& ByteBuffer::operator=(ByteBuffer&& other) noexcept
    {
        if (this != &other)
        {
            buffer_ = std::move(other.buffer_);
            assert(other.buffer_ == nullptr);
            readerIndex_ = other.readerIndex_;
            writerIndex_ = other.writerIndex_;
            capacity_ = other.capacity_;
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
        writeBytes(&value, kBooleanSize);
    }

    void ByteBuffer::writeByte(char value)
    {
        writeBytes(&value, kCharSize);
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

    void ByteBuffer::writeString(const StringType& message)
    {
        writeBytes(message.data(), message.length());
    }

    void ByteBuffer::writeByteBuffer(const ByteBuffer& byteBuffer)
    {
        writeBytes(&byteBuffer.buffer_[readerIndex_], byteBuffer.readableBytes());
    }

    void ByteBuffer::writeInt8(Int8Type value)
    {
        writeBytes(&value, kInt8Size);
    }

    void ByteBuffer::writeUint8(Uint8Type value)
    {
        writeBytes(&value, kInt8Size);
    }

    void ByteBuffer::writeInt16(Int16Type value)
    {
        Int16Type tmp = htobe16(value);
        writeBytes(&tmp, kInt16Size);
    }

    void ByteBuffer::writeUint16(Uint16Type value)
    {
        Uint16Type tmp = htobe16(value);
        writeBytes(&tmp, kInt16Size);
    }

    void ByteBuffer::writeInt32(Int32Type value)
    {
        Int32Type tmp = htobe32(value);
        writeBytes(&tmp, kInt32Size);
    }

    void ByteBuffer::writeUint32(Uint32Type value)
    {
        Uint32Type tmp = htobe32(value);
        writeBytes(&tmp, kInt32Size);
    }

    void ByteBuffer::writeInt64(Int64Type value)
    {
        Int64Type tmp = htobe64(value);
        writeBytes(&tmp, kInt64Size);
    }

    void ByteBuffer::writeUint64(Uint64Type value)
    {
        Uint64Type tmp = htobe64(value);
        writeBytes(&tmp, kInt64Size);
    }

    void ByteBuffer::writeFloat(float value)
    {
        writeBytes(&value, kFloatSize);
    }

    void ByteBuffer::writeDouble(double value)
    {
        writeBytes(&value, kDoubleSize);
    }

    bool ByteBuffer::readBoolean()
    {
        checkReadableBytes(kBooleanSize);
        bool val = false;
        ::memcpy(&val, &buffer_[readerIndex_], kBooleanSize);
        adjustReaderIndex(kBooleanSize);
        return val;
    }

    char ByteBuffer::readByte()
    {
        checkReadableBytes(kCharSize);
        char c = 0;
        ::memcpy(&c, &buffer_[readerIndex_], kCharSize);
        adjustReaderIndex(kCharSize);
        return c;
    }

    StringType ByteBuffer::readBytes(SizeType length)
    {
        checkReadableBytes(length);
        StringType s(&buffer_[readerIndex_], length);
        adjustReaderIndex(length);
        return s;
    }

    Int8Type ByteBuffer::readInt8()
    {
        checkReadableBytes(kInt8Size);
        Int8Type val = 0;
        ::memcpy(&val, &buffer_[readerIndex_], kInt8Size);
        adjustReaderIndex(kInt8Size);
        return val;
    }

    Uint8Type ByteBuffer::readUint8()
    {
        checkReadableBytes(kInt8Size);
        Uint8Type val = 0;
        ::memcpy(&val, &buffer_[readerIndex_], kInt8Size);
        adjustReaderIndex(kInt8Size);
        return val;
    }

    Int16Type ByteBuffer::readInt16()
    {
        checkReadableBytes(kInt16Size);
        Int16Type val = 0;
        ::memcpy(&val, &buffer_[readerIndex_], kInt16Size);
        adjustReaderIndex(kInt16Size);
        return be16toh(val);
    }

    Uint16Type ByteBuffer::readUint16()
    {
        checkReadableBytes(kInt16Size);
        Uint16Type val = 0;
        ::memcpy(&val, &buffer_[readerIndex_], kInt16Size);
        adjustReaderIndex(kInt16Size);
        return be16toh(val);
    }

    Int32Type ByteBuffer::readInt32()
    {
        checkReadableBytes(kInt32Size);
        Int32Type val = 0;
        ::memcpy(&val, &buffer_[readerIndex_], kInt32Size);
        adjustReaderIndex(kInt32Size);
        return be32toh(val);
    }

    Uint32Type ByteBuffer::readUint32()
    {
        checkReadableBytes(kInt32Size);
        Uint32Type val = 0;
        ::memcpy(&val, &buffer_[readerIndex_], kInt32Size);
        adjustReaderIndex(kInt32Size);
        return be32toh(val);
    }

    Int64Type ByteBuffer::readInt64()
    {
        checkReadableBytes(kInt64Size);
        Int64Type val = 0L;
        ::memcpy(&val, &buffer_[readerIndex_], kInt64Size);
        adjustReaderIndex(kInt64Size);
        return be64toh(val);
    }

    Uint64Type ByteBuffer::readUint64()
    {
        checkReadableBytes(kInt64Size);
        Uint64Type val = 0L;
        ::memcpy(&val, &buffer_[readerIndex_], kInt64Size);
        adjustReaderIndex(kInt64Size);
        return be64toh(val);
    }

    float ByteBuffer::readFloat()
    {
        checkReadableBytes(kFloatSize);
        float val = 0;
        ::memcpy(&val, &buffer_[readerIndex_], kFloatSize);
        adjustReaderIndex(kFloatSize);
        return *(float*) &val;
    }

    double ByteBuffer::readDouble()
    {
        checkReadableBytes(kDoubleSize);
        double val = 0;
        ::memcpy(&val, &buffer_[readerIndex_], kDoubleSize);
        adjustReaderIndex(kDoubleSize);
        return *(double*) &val;
    }

    StringType ByteBuffer::toString() const
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
                if (newCapacity > kMaxCapacity)
                {
                    THROW_FMT(OutOfMemoryException, "ByteBuffer newCapacity %lu exceeds the MaxCapacity", newCapacity);
                }
            }
            else
            {
                newCapacity = calculateNewCapacity(writeLen);
            }
            adjustCapacity(newCapacity);
        }
    }

    SizeType ByteBuffer::calculateNewCapacity(SizeType targetCapacity) const
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
            THROW_FMT(IndexOutOfBoundsException, "ByteBuffer readableBytes less than bytes %lu", bytes);
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
} // namespace nets