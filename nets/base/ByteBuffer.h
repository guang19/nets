//
// Created by n021949 on 2022/5/7.
//

#ifndef NETS_BYTEBUFFER_H
#define NETS_BYTEBUFFER_H

#include <cstdint>
#include <cstring>
#include <memory>

#include "nets/base/Copyable.h"

namespace nets::base
{
	class ByteBuffer : public Copyable
	{
	public:
		using SizeType = uint32_t;
		using CharPtr = ::std::unique_ptr<char[]>;

	public:
		explicit ByteBuffer(SizeType capacity);
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
			return (writerIndex_ - readerIndex_) > 0;
		}

		inline bool isWriteable() const
		{
			return (capacity_ - writerIndex_) > 0;
		}

		inline SizeType capacity() const
		{
			return capacity_;
		}

	protected:
		CharPtr buffer_ {nullptr};
		// reader pointer
		SizeType readerIndex_ {0};
		// writer pointer
		SizeType writerIndex_ {0};
		SizeType capacity_ {0};
	};
} // namespace nets::base

#endif // NETS_BYTEBUFFER_H
