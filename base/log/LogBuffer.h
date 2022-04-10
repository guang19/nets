//
// Created by YangGuang on 2022/4/3.
//

#ifndef NETS_LOGBUFFER_H
#define NETS_LOGBUFFER_H

#include <string>
#include "base/Noncopyable.h"

namespace nets
{
	namespace base
	{
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

				void clear();

				uint32_t available() const
				{
					return capacity_ - usedLen_;
				}

				uint32_t length() const
				{
					return usedLen_;
				}

				void append(const char *data, uint32_t len);

				LogBuffer& operator<<(int16_t n);
				LogBuffer& operator<<(uint16_t n);
				LogBuffer& operator<<(int32_t);
				LogBuffer& operator<<(uint32_t);
				LogBuffer& operator<<(int64_t);
				LogBuffer& operator<<(uint64_t);
				LogBuffer& operator<<(float n);
				LogBuffer& operator<<(double);
				LogBuffer& operator<<(char c);
				LogBuffer& operator<<(const char* str);
				LogBuffer& operator<<(const ::std::string& str);
				LogBuffer& operator<<(const LogBuffer& stream);

			private:
				uint32_t capacity_ { 0 };
				uint32_t usedLen_ { 0 };
				char *buffer_ { nullptr };
		};
	} // namespace base
} // namespace nets

#endif //NETS_LOGBUFFER_H
