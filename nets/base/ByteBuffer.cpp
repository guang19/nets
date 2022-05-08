//
// Created by n021949 on 2022/5/7.
//

#include "nets/base/ByteBuffer.h"

#include <algorithm>

namespace nets
{
	namespace base
	{
		ByteBuffer::ByteBuffer(SizeTp capacity) : buffer_(new char[capacity]), readerIndex_(0), writerIndex_(0),
			capacity_(capacity)
		{
			::memset(buffer_, 0, capacity);
		}

		ByteBuffer::~ByteBuffer()
		{
			if (buffer_ != nullptr)
			{
				delete[] buffer_;
				buffer_ = nullptr;
			}
		}

		ByteBuffer::ByteBuffer(const ByteBuffer& other)
		{
			readerIndex_ = other.readerIndex_;
			writerIndex_ = other.writerIndex_;
			capacity_ = other.capacity_;
			buffer_ = new char[capacity_];
			::memset(buffer_, 0, capacity_);
			::memcpy(buffer_, other.buffer_, capacity_);
		}

		ByteBuffer::ByteBuffer(ByteBuffer&& other) noexcept
		{
			readerIndex_ = other.readerIndex_;
			writerIndex_ = other.writerIndex_;
			capacity_ = other.capacity_;
			buffer_ = other.buffer_;
			other.buffer_ = nullptr;
		}

		ByteBuffer& ByteBuffer::operator=(const ByteBuffer& other)
		{
			if (this != &other)
			{
				readerIndex_ = other.readerIndex_;
				writerIndex_ = other.writerIndex_;
				capacity_ = other.capacity_;
				if (buffer_ != nullptr)
				{
					delete[] buffer_;
				}
				buffer_ = new char[capacity_];
				::memset(buffer_, 0, capacity_);
				::memcpy(buffer_, other.buffer_, capacity_);
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
				if (buffer_ != nullptr)
				{
					delete[] buffer_;
				}
				buffer_ = other.buffer_;
				other.buffer_ = nullptr;
			}
			return *this;
		}

		void ByteBuffer::swap(ByteBuffer &&other)
		{
			::std::swap(buffer_, other.buffer_);
			::std::swap(readerIndex_, other.readerIndex_);
			::std::swap(writerIndex_, other.writerIndex_);
			::std::swap(capacity_, other.capacity_);
		}
	} // namespace base
} // namespace nets