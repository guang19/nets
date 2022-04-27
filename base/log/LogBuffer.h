//
// Created by YangGuang on 2022/4/3.
//

#ifndef NETS_LOGBUFFER_H
#define NETS_LOGBUFFER_H

#include <cstdint>
#include <string>
#include "base/Noncopyable.h"

namespace nets
{
	namespace base
	{
		class LogBuffer
		{
			public:
				LogBuffer();
				explicit LogBuffer(uint32_t logBufferSize);
				~LogBuffer();

				inline const char* getBuffer() const
				{
					return buffer_;
				}

				inline char* getCurrentBuffer()
				{
					return buffer_ + usedLen_;
				}

				inline uint32_t available() const
				{
					return capacity_ - usedLen_;
				}

				inline uint32_t length() const
				{
					return usedLen_;
				}

				inline void addLen(uint32_t len)
				{
					usedLen_ += len;
				}

				void append(const char *data, uint32_t len);
				void appendPointer(uintptr_t ptr);

				template <typename Number>
				void appendInteger(Number n);

				template <typename Float>
				void appendFloat(Float n);

				LogBuffer& operator<<(int16_t n);
				LogBuffer& operator<<(uint16_t n);
				LogBuffer& operator<<(int32_t n);
				LogBuffer& operator<<(uint32_t n);
				LogBuffer& operator<<(int64_t n);
				LogBuffer& operator<<(uint64_t n);
				LogBuffer& operator<<(const void* ptr);
				LogBuffer& operator<<(float n);
				LogBuffer& operator<<(double n);
				LogBuffer& operator<<(char c);
				LogBuffer& operator<<(const char* str);
				LogBuffer& operator<<(const ::std::string& str);
				LogBuffer& operator<<(const LogBuffer& stream);

			private:
				char* buffer_ { nullptr };
				uint32_t capacity_ { 0 };
				uint32_t usedLen_ { 0 };
		};
	} // namespace base
} // namespace nets

#endif //NETS_LOGBUFFER_H
