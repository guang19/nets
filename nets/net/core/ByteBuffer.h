//
// Created by guang19 on 2022/5/7.
//

#ifndef NETS_NET_BYTEBUFFER_H
#define NETS_NET_BYTEBUFFER_H

#include <cstdint>
#include <memory>

#include "nets/base/Copyable.h"

namespace nets::net
{
    class ByteBuffer : public nets::base::Copyable
    {
    public:
        using SizeType = uint32_t;
        using CharArrayPtr = ::std::unique_ptr<char[]>;

    public:
        ByteBuffer();
        explicit ByteBuffer(SizeType initialCapacity);
        virtual ~ByteBuffer() = default;

        ByteBuffer(const ByteBuffer& other);
        ByteBuffer(ByteBuffer&& other) noexcept;
        ByteBuffer& operator=(const ByteBuffer& other);
        ByteBuffer& operator=(ByteBuffer&& other) noexcept;

    public:
        inline SizeType readerIndex() const
        {
            return readerIndex_;
        }

        inline SizeType discardBytes() const
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

        inline SizeType capacity() const
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
        void writeBytes(const void* data, SizeType len);
        void writeBytes(const char* data, SizeType len);
        void writeInt8(int8_t value);
        void writeInt16(int16_t value);
        void writeInt32(int32_t value);
        void writeInt64(int64_t value);
        void writeFloat(float value);
        void writeDouble(double value);

        bool readBoolean();
        char readByte();
        ::std::string readBytes(SizeType len);
        int8_t readInt8();
        int16_t readInt16();
        int32_t readInt32();
        int64_t readInt64();
        float readFloat();
        double readDouble();

    private:
        void ensureWritable(SizeType writeLen);
        SizeType calculateNewCapacity(SizeType targetCapacity);
        void adjustCapacity(SizeType newCapacity);

        void checkReadableBytes(SizeType bytes);
        void adjustReaderIndex(SizeType bytes);

    private:
        CharArrayPtr buffer_ {nullptr};
        // reader pointer
        SizeType readerIndex_ {0};
        // writer pointer
        SizeType writerIndex_ {0};
        SizeType capacity_ {0};
    };
} // namespace nets::net

#endif // NETS_NET_BYTEBUFFER_H
