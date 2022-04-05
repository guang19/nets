//
// Created by YangGuang on 2022/4/3.
//

#ifndef NETS_LOGBUFFERLIST_H
#define NETS_LOGBUFFERLIST_H

#include <stdint.h>
#include <string>
#include <string.h>
#include "base/Noncopyable.h"

namespace nets
{
	namespace base
	{
		class LogBufferList
		{
			private:
				class LogBuffer : Noncopyable
				{
					public:
						explicit LogBuffer(uint32_t capacity);
						~LogBuffer();

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

						uint32_t available() const
						{
							return capacity_ - usedLen_;
						}

						uint32_t length() const
						{
							return usedLen_;
						}

						void clear();
						bool append(const char *data, uint32_t len);
					private:
						uint32_t capacity_;
						uint32_t usedLen_;
						char *buffer_;
				};

			private:
				LogBuffer logBuffer;
		};
	} // namespace base
} // namespace nets

#endif //NETS_LOGBUFFERLIST_H
