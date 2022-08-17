//
// Created by guang19
//

#ifndef NETS_NET_BYTE_BUFFER_H
#define NETS_NET_BYTE_BUFFER_H

#include <cstdint>
#include <memory>

#include "nets/base/Copyable.h"
#include "nets/net/core/Socket.h"

namespace nets::net
{
    class SocketChannel;

    class ByteBuffer : public nets::base::Copyable
    {
    public:
        using SizeType = ::size_t;
        using StringType = ::std::string;
        using CharArrayPtr = ::std::unique_ptr<char[]>;

    public:
        ByteBuffer();
        explicit ByteBuffer(SizeType initialCapacity);
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
        void writeBytes(const void* data, SizeType length);
        void writeBytes(const char* data, SizeType length);
        SSizeType writeBytes(const SocketChannel& channel, SizeType length);
        void writeInt8(int8_t value);
        void writeInt16(int16_t value);
        void writeInt32(int32_t value);
        void writeInt64(int64_t value);
        void writeFloat(float value);
        void writeDouble(double value);

        bool readBoolean();
        char readByte();
        StringType readBytes(SizeType length);
        int8_t readInt8();
        int16_t readInt16();
        int32_t readInt32();
        int64_t readInt64();
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
        CharArrayPtr buffer_ {nullptr};
        // reader pointer
        SizeType readerIndex_ {0};
        // writer pointer
        SizeType writerIndex_ {0};
        SizeType capacity_ {0};
    };
} // namespace nets::net

#endif // NETS_NET_BYTE_BUFFER_H