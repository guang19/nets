//
// Created by guang19 on 2022/6/9.
//

#ifndef NETS_BASE_STACK_BUFFER_H
#define NETS_BASE_STACK_BUFFER_H

#include <cstdint>

#include "nets/base/CommonMacro.h"
#include "nets/base/Noncopyable.h"
#include "nets/base/StringUtils.h"

namespace nets::base
{
	template <uint32_t SIZE>
	class StackBuffer : Noncopyable
	{
	public:
		StackBuffer() : writerIndex_(0), capacity_(SIZE)
		{
			MEMZERO(buffer_, SIZE);
		}

		~StackBuffer() = default;

	public:
		inline const char* carray() const
		{
			return buffer_;
		}

		inline uint32_t len() const
		{
			return writerIndex_;
		}

		inline uint32_t writerIndex() const
		{
			return writerIndex_;
		}

		inline uint32_t writeableBytes() const
		{
			return capacity_ - writerIndex_;
		}

		void append(const char* data, uint32_t len);

		void appendPointer(const void* ptr);

		template <typename Number>
		void appendInteger(Number n);

		template <typename Float>
		void appendFloat(Float f);

	private:
		char buffer_[SIZE] {0};
		// writer pointer
		uint32_t writerIndex_ {0};
		uint32_t capacity_ {SIZE};
	};

	template <uint32_t SIZE>
	void StackBuffer<SIZE>::append(const char* data, uint32_t len)
	{
		if (writeableBytes() > len)
		{
			::memcpy(buffer_ + writerIndex_, data, len);
			writerIndex_ += len;
		}
	}

	template <uint32_t SIZE>
	void StackBuffer<SIZE>::appendPointer(const void* ptr)
	{
		if (writeableBytes() > MaxNumLen)
		{
			writerIndex_ += utils::fromHex(buffer_ + writerIndex_, reinterpret_cast<uintptr_t>(ptr));
		}
	}

	template <uint32_t SIZE>
	template <typename Number>
	void StackBuffer<SIZE>::appendInteger(Number n)
	{
		if (writeableBytes() > MaxNumLen)
		{
			writerIndex_ += utils::fromInt(buffer_ + writerIndex_, n);
		}
	}

	template <uint32_t SIZE>
	template <typename Float>
	void StackBuffer<SIZE>::appendFloat(Float f)
	{
		if (writeableBytes() > MaxFloatLen)
		{
			writerIndex_ += utils::fromFloat(buffer_ + writerIndex_, f);
		}
	}
} // namespace nets::base

#endif // NETS_BASE_STACK_BUFFER_H
