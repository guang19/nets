//
// Created by YangGuang on 2022/4/3.
//

#include "base/log/LogBuffer.h"
#include <string.h>

namespace nets
{
	namespace base
	{
		LogBuffer::LogBuffer(uint32_t capacity) : capacity_(capacity), usedLen_(0)
		{
			buffer_ = new char[capacity_];
			memset(buffer_, 0, capacity_);
		}

		LogBuffer::~LogBuffer()
		{
			if (buffer_ != nullptr)
			{
				delete[] buffer_;
				buffer_ = nullptr;
			}
		}

		void LogBuffer::clear()
		{
			memset(buffer_, 0, usedLen_);
			usedLen_ = 0;
		}

		void LogBuffer::append(const char *data, uint32_t len)
		{
			if (available() > len)
			{
				memcpy(buffer_ + usedLen_, data, len);
				usedLen_ += len;
			}
			else
			{
				char* newBuffer = nullptr;
				try
				{
					uint32_t minCapacity = capacity_ + len + 1;
					uint32_t newCapacity = capacity_ + (capacity_ >> 1);
					if (newCapacity < minCapacity)
					{
						newCapacity = minCapacity;
					}
					newBuffer = new char[newCapacity];
					memcpy(newBuffer, buffer_, usedLen_);
					memcpy(newBuffer + usedLen_, data, len);
					delete[] buffer_;
					buffer_ = newBuffer;
					usedLen_ += len;
					capacity_ = newCapacity;
				}
				catch (...)
				{
					if (newBuffer != nullptr)
					{
						delete[] newBuffer;
						newBuffer = nullptr;
					}
					abort();
				}
			}
		}

		LogBuffer& LogBuffer::operator<<(int16_t n)
		{
			this->operator<<(static_cast<int32_t>(n));
			return *this;
		}

		LogBuffer& LogBuffer::operator<<(uint16_t n)
		{
			this->operator<<(static_cast<uint32_t>(n));
			return *this;
		}

		LogBuffer& LogBuffer::operator<<(int32_t n)
		{
			this->operator<<(::std::to_string(n));
			return *this;
		}

		LogBuffer& LogBuffer::operator<<(uint32_t n)
		{
			this->operator<<(::std::to_string(n));
			return *this;
		}

		LogBuffer& LogBuffer::operator<<(int64_t n)
		{
			this->operator<<(::std::to_string(n));
			return *this;
		}

		LogBuffer& LogBuffer::operator<<(uint64_t n)
		{
			this->operator<<(::std::to_string(n));
			return *this;
		}

		LogBuffer& LogBuffer::operator<<(float n)
		{
			this->operator<<(::std::to_string(n));
			return *this;
		}

		LogBuffer& LogBuffer::operator<<(double n)
		{
			this->operator<<(::std::to_string(n));
			return *this;
		}

		LogBuffer& LogBuffer::operator<<(char c)
		{
			append(&c, 1);
			return *this;
		}

		LogBuffer& LogBuffer::operator<<(const char* str)
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

		LogBuffer& LogBuffer::operator<<(const LogBuffer& other)
		{
			append(other.buffer_, usedLen_);
			return *this;
		}
	} // namespace base
} // namespace nets