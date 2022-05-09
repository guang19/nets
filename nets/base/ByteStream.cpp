//
// Created by YangGuang on 2022/5/8.
//

#include "nets/base/ByteStream.h"

#include <algorithm>
#include <cmath>
#include <limits>

namespace nets
{
	namespace base
	{
		ByteStream::ByteStream(ByteBuffer::SizeTp capacity) : ByteBuffer(capacity)
		{
		}

		namespace
		{
			constexpr char Digits[] = {"0123456789"};
			constexpr char HexDigits[] = {"0123456789abcdef"};
			constexpr uint32_t MaxNumSize = ::std::numeric_limits<uint64_t>::digits10 + 6;
		}

		template <typename Number>
		uint32_t intToStr(char* buffer, Number n)
		{
			char* tmp = buffer;
			do
			{
				uint16_t lstIdx = static_cast<uint16_t>(n % 10);
				*tmp = Digits[lstIdx];
				++tmp;
				n /= 10;
			}
			while (n > 0);
			if (n < 0)
			{
				*tmp = '-';
				++tmp;
			}
			::std::reverse(buffer, tmp);
			return tmp - buffer;
		}

		template <typename Number>
		uint32_t hexToStr(char* buffer, Number n)
		{
			char* tmp = buffer;
			tmp[0] = '0';
			tmp[1] = 'x';
			tmp += 2;
			do
			{
				uint16_t lstIdx = static_cast<uint16_t>(n % 16);
				*tmp = HexDigits[lstIdx];
				++tmp;
				n /= 16;
			}
			while (n > 0);
			::std::reverse(buffer + 2, tmp);
			return tmp - buffer;
		}

		template <typename Number>
		uint32_t convertToStr(char* buffer, Number n, uint16_t base)
		{
			if (base == 10)
			{
				return intToStr(buffer, n);
			}
			else if (base == 16)
			{
				return hexToStr(buffer, n);
			}
			return 0;
		}

		template <typename Number>
		void ByteStream::appendInteger(Number n)
		{
			if (writeableBytes() > MaxNumSize)
			{
				writerIndex_ += convertToStr(buffer_ + writerIndex_, n, 10);
			}
		}

		void ByteStream::appendPointer(uintptr_t ptr)
		{
			if (writeableBytes() >= MaxNumSize)
			{
				writerIndex_ += convertToStr(buffer_ + writerIndex_, ptr, 16);
			}
		}

		template<typename Float>
		void ByteStream::appendFloat(Float f)
		{
			if (writeableBytes() > MaxNumSize)
			{
				char* cur = buffer_ + writerIndex_;
				if (::std::isnan(f))
				{
					::memcpy(cur, "nan", 3);
					writerIndex_ += 3;
				}
				else if (::std::isinf(f))
				{
					::memcpy(cur, "inf", 3);
					writerIndex_ += 3;
				}
				else
				{
					writerIndex_ += ::snprintf(cur, MaxNumSize, "%.18g", f);
				}
			}
		}

		ByteStream& ByteStream::operator<<(char c)
		{
			appendStr(&c, 1);
			return *this;
		}

		ByteStream& ByteStream::operator<<(int8_t n)
		{
			appendInteger(n);
			return *this;
		}

		ByteStream& ByteStream::operator<<(uint8_t n)
		{
			appendInteger(n);
			return *this;
		}

		ByteStream& ByteStream::operator<<(int16_t n)
		{
			appendInteger(n);
			return *this;
		}

		ByteStream& ByteStream::operator<<(uint16_t n)
		{
			appendInteger(n);
			return *this;
		}

		ByteStream& ByteStream::operator<<(int32_t n)
		{
			appendInteger(n);
			return *this;
		}

		ByteStream& ByteStream::operator<<(uint32_t n)
		{
			appendInteger(n);
			return *this;
		}

		ByteStream& ByteStream::operator<<(int64_t n)
		{
			appendInteger(n);
			return *this;
		}

		ByteStream& ByteStream::operator<<(uint64_t n)
		{
			appendInteger(n);
			return *this;
		}

		ByteStream& ByteStream::operator<<(const void* ptr)
		{
			appendPointer(reinterpret_cast<uintptr_t>(ptr));
			return *this;
		}

		ByteStream& ByteStream::operator<<(float f)
		{
			appendFloat(f);
			return *this;
		}

		ByteStream& ByteStream::operator<<(double f)
		{
			appendFloat(f);
			return *this;
		}

		void ByteStream::appendStr(const char* data, uint32_t len)
		{
			if (writeableBytes() > len)
			{
				::memcpy(buffer_ + writerIndex_, data, len);
				writerIndex_ += len;
			}
		}

		ByteStream& ByteStream::operator<<(const char* str)
		{
			appendStr(str, ::strlen(str));
			return *this;
		}

		ByteStream& ByteStream::operator<<(const ::std::string& str)
		{
			appendStr(str.c_str(), str.length());
			return *this;
		}

		ByteStream& ByteStream::operator<<(const ByteStream& stream)
		{
			appendStr(stream.buffer_, stream.writerIndex_);
			return *this;
		}

		ByteStream::ByteStream(const ByteStream& stream) : ByteBuffer(stream)
		{
		}

		ByteStream::ByteStream(ByteStream&& stream) noexcept : ByteBuffer(::std::move(stream))
		{
		}

		ByteStream &ByteStream::operator=(const ByteStream& stream)
		{
			ByteBuffer::operator=(stream);
			return *this;
		}

		ByteStream &ByteStream::operator=(ByteStream&& stream) noexcept
		{
			ByteBuffer::operator=(stream);
			return *this;
		}
	} // namespace base
} // namespace nets