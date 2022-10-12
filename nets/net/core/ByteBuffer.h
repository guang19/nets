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

#ifndef NETS_BYTE_BUFFER_H
#define NETS_BYTE_BUFFER_H

#include <cstdint>
#include <memory>

#include "nets/base/Copyable.h"
#include "nets/net/core/Socket.h"

namespace nets
{
    class SocketChannel;
    class DatagramChannel;

    class ByteBuffer : public Copyable
    {
    public:
        using SizeType = ::size_t;
        using StringType = ::std::string;
        using CharArrayPtr = ::std::unique_ptr<char[]>;

    public:
        ByteBuffer();
        explicit ByteBuffer(SizeType initialCapacity);
        explicit ByteBuffer(const char* data);
        explicit ByteBuffer(const void* data, SizeType length);
        ~ByteBuffer() = default;

        ByteBuffer(const ByteBuffer& other);
        ByteBuffer(ByteBuffer&& other) noexcept;
        ByteBuffer& operator=(const ByteBuffer& other);
        ByteBuffer& operator=(ByteBuffer&& other) noexcept;

    public:
        inline SizeType readerIndex() const
        {
            return readerIndex_;
        }

        inline SizeType discardReadBytes() const
        {
            return readerIndex_;
        }

        inline SizeType writerIndex() const
        {
            return writerIndex_;
        }

        inline SizeType readableBytes() const
        {
            return writerIndex_ - readerIndex_;
        }

        inline SizeType writableBytes() const
        {
            return capacity_ - writerIndex_;
        }

        inline bool isReadable() const
        {
            return writerIndex_ > readerIndex_;
        }

        inline bool isWritable() const
        {
            return capacity_ > writerIndex_;
        }

        inline void setReaderIndex(SizeType readerIndex)
        {
            readerIndex_ = readerIndex;
        }

        inline void setWriterIndex(SizeType writerIndex)
        {
            writerIndex_ = writerIndex;
        }

        inline SizeType capacity() const
        {
            return capacity_;
        }

        inline char* data()
        {
            return &buffer_[readerIndex_];
        }

        inline const char* data() const
        {
            return &buffer_[readerIndex_];
        }

        inline void clear()
        {
            readerIndex_ = writerIndex_ = 0;
        }

    public:
        void swap(ByteBuffer&& other);
        void writeBoolean(bool value);
        void writeByte(char value);
        void writeBytes(const char* data);
        void writeBytes(const void* data, SizeType length);
        void writeBytes(const char* data, SizeType length);
        SSizeType writeBytes(const SocketChannel& channel, SizeType length);
        SSizeType writeBytes(const DatagramChannel& channel, SizeType length, InetSockAddress& srcAddr);
        void writeInt8(::int8_t value);
        void writeUint8(::uint8_t value);
        void writeInt16(::int16_t value);
        void writeUint16(::uint16_t value);
        void writeInt32(::int32_t value);
        void writeUint32(::uint32_t value);
        void writeInt64(::int64_t value);
        void writeUint64(::uint64_t value);
        void writeFloat(float value);
        void writeDouble(double value);

        bool readBoolean();
        char readByte();
        StringType readBytes(SizeType length);
        ::int8_t readInt8();
        ::int16_t readInt16();
        ::int32_t readInt32();
        ::int64_t readInt64();
        float readFloat();
        double readDouble();
        StringType toString() const;

    private:
        void ensureWritable(SizeType writeLen);
        SizeType calculateNewCapacity(SizeType targetCapacity) const;
        void adjustCapacity(SizeType newCapacity);

        void checkReadableBytes(SizeType bytes) const;
        void adjustReaderIndex(SizeType bytes);

    private:
        CharArrayPtr buffer_;
        // reader pointer
        SizeType readerIndex_;
        // writer pointer
        SizeType writerIndex_;
        SizeType capacity_;
    };
} // namespace nets

#endif // NETS_BYTE_BUFFER_H