//
// Created by YangGuang on 2022/4/3.
//

#include "base/log/LogBuffer.h"

#include <algorithm>
#include <cmath>
#include <cstring>
#include <limits>

namespace nets
{
	namespace base
	{

		namespace
		{
			// log length limit: 8K
			constexpr uint32_t DefaultLogBufferSize = (1024 << 3);
		}

		LogBuffer::LogBuffer() : LogBuffer(DefaultLogBufferSize)
		{
		}

		LogBuffer::LogBuffer(uint32_t logBufferSize) : buffer_(new char[logBufferSize]),
			capacity_(logBufferSize), usedLen_(0)
		{
			::std::memset(buffer_, 0, logBufferSize);
		}

		LogBuffer::~LogBuffer()
		{
			if (buffer_ != nullptr)
			{
				delete[] buffer_;
				buffer_ = nullptr;
			}
		}

		void LogBuffer::append(const char *data, uint32_t len)
		{
			if (available() > len)
			{
				::std::memcpy(buffer_ + usedLen_, data, len);
				usedLen_ += len;
			}
		}

		namespace
		{
			constexpr char Digits[] = {"0123456789"};
			constexpr char HexDigits[] = {"0123456789abcdef"};
			constexpr uint32_t MaxNumSize = ::std::numeric_limits<uint64_t>::digits10 + 6;
		}

		template<typename Number>
		uint32_t intToCstr(char *buffer, Number n, uint16_t base)
		{
			char *tmp = buffer;
			if (base == 10)
			{
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
			}
			else if (base == 16)
			{
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
			}
			return tmp - buffer;
		}

		template<typename Number>
		void LogBuffer::appendInteger(Number n)
		{
			if (available() >= MaxNumSize)
			{
				usedLen_ += intToCstr(buffer_ + usedLen_, n, 10);
			}
		}

		void LogBuffer::appendPointer(uintptr_t ptr)
		{
			if (available() >= MaxNumSize)
			{
				usedLen_ += intToCstr(buffer_ + usedLen_, ptr, 16);
			}
		}

		template<typename Float>
		void LogBuffer::appendFloat(Float n)
		{
			if (available() >= MaxNumSize)
			{
				char *cur = buffer_ + usedLen_;
				if (::std::isnan(n))
				{
					::std::memcpy(cur, "nan", 3);
					usedLen_ += 3;
				}
				else if (::std::isinf(n))
				{
					::std::memcpy(cur, "inf", 3);
					usedLen_ += 3;
				}
				else
				{
					usedLen_ += snprintf(cur, MaxNumSize, "%.18g", n);
				}
			}
		}

		LogBuffer& LogBuffer::operator<<(int16_t n)
		{
			appendInteger(n);
			return *this;
		}

		LogBuffer& LogBuffer::operator<<(uint16_t n)
		{
			appendInteger(n);
			return *this;
		}

		LogBuffer& LogBuffer::operator<<(int32_t n)
		{
			appendInteger(n);
			return *this;
		}

		LogBuffer& LogBuffer::operator<<(uint32_t n)
		{
			appendInteger(n);
			return *this;
		}

		LogBuffer& LogBuffer::operator<<(int64_t n)
		{
			appendInteger(n);
			return *this;
		}

		LogBuffer& LogBuffer::operator<<(uint64_t n)
		{
			appendInteger(n);
			return *this;
		}

		LogBuffer& LogBuffer::operator<<(const void *ptr)
		{
			appendPointer(reinterpret_cast<uintptr_t>(ptr));
			return *this;
		}

		LogBuffer& LogBuffer::operator<<(float n)
		{
			appendFloat(n);
			return *this;
		}

		LogBuffer& LogBuffer::operator<<(double n)
		{
			appendFloat(n);
			return *this;
		}

		LogBuffer& LogBuffer::operator<<(char c)
		{
			append(&c, 1);
			return *this;
		}

		LogBuffer& LogBuffer::operator<<(const char *str)
		{
			if (str != nullptr)
			{
				append(str, strlen(str));
			}
			return *this;
		}

		LogBuffer& LogBuffer::operator<<(const ::std::string& str)
		{
			append(str.c_str(), str.length());
			return *this;
		}

		LogBuffer& LogBuffer::operator<<(const LogBuffer &other)
		{
			append(other.buffer_, other.usedLen_);
			return *this;
		}
	} // namespace base
} // namespace nets