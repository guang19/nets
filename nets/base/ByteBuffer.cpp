//
// Created by n021949 on 2022/5/7.
//

#include "nets/base/ByteBuffer.h"

#include <cstring>

namespace nets
{
	namespace base
	{
		base::ByteBuffer::ByteBuffer(SizeTp capacity) : buffer_(new char[capacity]), readerIndex_(0), writerIndex_(0),
			capacity_(capacity)
		{
			::std::memset(buffer_, 0, capacity);
		}

		base::ByteBuffer::~ByteBuffer()
		{
			if (buffer_ != nullptr)
			{
				delete[] buffer_;
				buffer_ = nullptr;
			}
		}
	} // namespace base
} // namespace nets