//
// Created by YangGuang on 2022/4/3.
//

#include "base/log/LogBufferList.h"

namespace nets
{
	namespace base
	{
		LogBufferList::LogBuffer::LogBuffer(uint32_t capacity) : capacity_(capacity), usedLen_(0)
		{
			buffer_ = new char[capacity];
			memset(buffer_, 0, capacity * sizeof(char));
		}

		LogBufferList::LogBuffer::~LogBuffer()
		{
			if (buffer_ != nullptr)
			{
				delete[] buffer_;
				buffer_ = nullptr;
			}
		}

		void LogBufferList::LogBuffer::clear()
		{
			memset(buffer_, 0, usedLen_ * sizeof(char));
			usedLen_ = 0;
		}

		bool LogBufferList::LogBuffer::append(const char *data, uint32_t len)
		{
			if (available() > len)
			{
				memcpy(buffer_ + usedLen_, data, len);
				usedLen_ += len;
				return true;
			}
			return false;
		}
	} // namespace base
} // namespace nets