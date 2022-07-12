//
// Created by guang19 on 2022/5/7.
//

#include "nets/base/ByteBuffer.h"

#include <algorithm>
#include <utility>

#include "nets/base/CommonMacro.h"

namespace nets::base
{
	ByteBuffer::ByteBuffer(SizeType capacity)
		: buffer_(::std::make_unique<char[]>(capacity)), readerIndex_(0), writerIndex_(0), capacity_(capacity)
	{
		MEMZERO(buffer_.get(), capacity_);
	}

	ByteBuffer::ByteBuffer(const ByteBuffer& other)
	{
		readerIndex_ = other.readerIndex_;
		writerIndex_ = other.writerIndex_;
		capacity_ = other.capacity_;
		buffer_.reset(new char[capacity_]);
		MEMZERO(buffer_.get(), capacity_);
		::memcpy(buffer_.get(), other.buffer_.get(), capacity_);
	}

	ByteBuffer::ByteBuffer(ByteBuffer&& other) noexcept
	{
		readerIndex_ = other.readerIndex_;
		writerIndex_ = other.writerIndex_;
		capacity_ = other.capacity_;
		buffer_ = ::std::move(other.buffer_);
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
			MEMZERO(buffer_.get(), capacity_);
			::memcpy(buffer_.get(), other.buffer_.get(), capacity_);
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
} // namespace nets::base