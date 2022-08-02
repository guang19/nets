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

        void swap(ByteBuffer&& other);

    public:
        inline SizeType readerIndex() const
        {
            return readerIndex_;
        }

        inline SizeType writerIndex() const
        {
            return writerIndex_;
        }

        inline void setIndex(SizeType readerIndex, SizeType writerIndex)
        {
            readerIndex_ = readerIndex;
            writerIndex_ = writerIndex;
        }

        inline void setReaderIndex(SizeType readerIndex)
        {
            readerIndex_ = readerIndex;
        }

        inline void setWriterIndex(SizeType writerIndex)
        {
            writerIndex_ = writerIndex;
        }

        inline SizeType readableBytes() const
        {
            return writerIndex_ - readerIndex_;
        }

        inline SizeType writeableBytes() const
        {
            return capacity_ - writerIndex_;
        }

        inline bool isReadable() const
        {
            return writerIndex_ > readerIndex_;
        }

        inline bool isWriteable() const
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
        void writeBytes(const char* data, SizeType len);

    private:
        void assureWritable(SizeType minWritableBytes);

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
