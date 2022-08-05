//
// Created by guang19 on 2022/5/7.
//

#ifndef NETS_NET_BYTEBUFFER_H
#define NETS_NET_BYTEBUFFER_H

#include <cstdint>
#include <memory>

#include "nets/base/Copyable.h"
#include "nets/net/core/SocketChannel.h"

namespace nets::net
{
    class ByteBuffer : public nets::base::Copyable
    {
    public:
        using IntType = uint32_t;
        using StringType = ::std::string;
        using CharArrayPtr = ::std::unique_ptr<char[]>;

    public:
        ByteBuffer();
        explicit ByteBuffer(IntType initialCapacity);
        virtual ~ByteBuffer() = default;

        ByteBuffer(const ByteBuffer& other);
        ByteBuffer(ByteBuffer&& other) noexcept;
        ByteBuffer& operator=(const ByteBuffer& other);
        ByteBuffer& operator=(ByteBuffer&& other) noexcept;

    public:
        inline IntType readerIndex() const
        {
            return readerIndex_;
        }

        inline IntType discardReadBytes() const
        {
            return readerIndex_;
        }

        inline IntType writerIndex() const
        {
            return writerIndex_;
        }

        inline IntType readableBytes() const
        {
            return writerIndex_ - readerIndex_;
        }

        inline IntType writableBytes() const
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

        inline IntType capacity() const
        {
            return capacity_;
        }

        inline void clear()
        {
            readerIndex_ = writerIndex_ = 0;
        }

    public:
        void swap(ByteBuffer&& other);
        void writeBoolean(bool value);
        void writeByte(char value);
        void writeBytes(const void* data, IntType len);
        void writeBytes(const char* data, IntType len);
        SSizeType writeBytes(SocketChannel& channel);
        void writeInt8(int8_t value);
        void writeInt16(int16_t value);
        void writeInt32(int32_t value);
        void writeInt64(int64_t value);
        void writeFloat(float value);
        void writeDouble(double value);

        bool readBoolean();
        char readByte();
        StringType readBytes(IntType len);
        int8_t readInt8();
        int16_t readInt16();
        int32_t readInt32();
        int64_t readInt64();
        float readFloat();
        double readDouble();

    private:
        void ensureWritable(IntType writeLen);
        IntType calculateNewCapacity(IntType targetCapacity) const;
        void adjustCapacity(IntType newCapacity);

        void checkReadableBytes(IntType bytes) const;
        void adjustReaderIndex(IntType bytes);

    private:
        CharArrayPtr buffer_ {nullptr};
        // reader pointer
        IntType readerIndex_ {0};
        // writer pointer
        IntType writerIndex_ {0};
        IntType capacity_ {0};
    };
} // namespace nets::net

#endif // NETS_NET_BYTEBUFFER_H
