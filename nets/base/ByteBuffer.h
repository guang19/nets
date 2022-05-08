//
// Created by n021949 on 2022/5/7.
//

#ifndef NETS_BYTEBUFFER_H
#define NETS_BYTEBUFFER_H

#include <cstdint>
#include <cstring>
#include "nets/base/Copyable.h"

namespace nets
{
	namespace base
	{
		class ByteBuffer : public Copyable
		{
			public:
				using SizeTp = uint32_t;

			public:
				explicit ByteBuffer(SizeTp capacity);
				virtual ~ByteBuffer();

				ByteBuffer(const ByteBuffer& other);
				ByteBuffer(ByteBuffer&& other) noexcept;
				ByteBuffer& operator=(const ByteBuffer& other);
				ByteBuffer& operator=(ByteBuffer&& other) noexcept;

			public:
				void swap(ByteBuffer&& other);

			public:
				inline SizeTp readerIndex() const
				{
					return readerIndex_;
				}

				inline SizeTp writerIndex() const
				{
					return writerIndex_;
				}

				inline void setIndex(SizeTp readerIndex, SizeTp writerIndex)
				{
					readerIndex_ = readerIndex;
					writerIndex_ = writerIndex;
				}

				inline void setReaderIndex(SizeTp readerIndex)
				{
					readerIndex_ = readerIndex;
				}

				inline void setWriterIndex(SizeTp writerIndex)
				{
					writerIndex_ = writerIndex;
				}

				inline SizeTp readableBytes() const
				{
					return writerIndex_ - readerIndex_;
				}

				inline SizeTp writeableBytes() const
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

				inline SizeTp capacity() const
				{
					return capacity_;
				}

			protected:
				char* buffer_ { nullptr };
				// reader pointer
				SizeTp readerIndex_ { 0 };
				// writer pointer
				SizeTp writerIndex_ { 0 };
				SizeTp capacity_ { 0 };
		};
	} // namespace base
} // namespace nets

#endif //NETS_BYTEBUFFER_H
