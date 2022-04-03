//
// Created by n021949 on 2022/4/2.
//

#ifndef NETS_LOGBUFFER_H
#define NETS_LOGBUFFER_H

#include <stdint.h>
#include <string.h>
#include <string>
#include "base/Noncopyable.h"

namespace nets
{
	namespace base
	{
		class LogBuffer : Noncopyable
		{
			public:
				explicit LogBuffer(uint32_t capacity) : capacity_(capacity), usedLen_(0)
				{
					buffer_ = new char[capacity];
					memset(buffer_, 0, capacity * sizeof(char));
				}

				~LogBuffer()
				{
					if (buffer_ != nullptr)
					{
						delete[] buffer_;
						buffer_ = nullptr;
					}
				}

				const char* getBuffer() const
				{
					return buffer_;
				}

				::std::string toString() const
				{
					return ::std::string(buffer_, usedLen_);
				}

				void reset()
				{
					usedLen_ = 0;
				}

				void clear()
				{
					memset(buffer_, 0, usedLen_ * sizeof(char));
					usedLen_ = 0;
				}

				uint32_t available() const
				{
					return capacity_ - usedLen_;
				}

				uint32_t length() const
				{
					return usedLen_;
				}

				bool append(const char* data, uint32_t len);

			private:
				uint32_t capacity_;
				uint32_t usedLen_;
				char* buffer_;
		};
	}
}

#endif //NETS_LOGBUFFER_H
