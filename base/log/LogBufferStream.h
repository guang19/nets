//
// Created by YangGuang on 2022/4/3.
//

#ifndef NETS_LOGBUFFERSTREAM_H
#define NETS_LOGBUFFERSTREAM_H

#include <string>
#include <cstdint>
#include "base/Noncopyable.h"

namespace nets
{
	namespace base
	{
		namespace
		{
			constexpr uint32_t LogBufferSize = (1024 << 6);
		}

		class LogBufferStream : Noncopyable
		{
			public:
				LogBufferStream() = default;
				~LogBufferStream() = default;

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

				LogBufferStream& operator<<(int16_t n);
				LogBufferStream& operator<<(uint16_t n);
				LogBufferStream& operator<<(int32_t n);
				LogBufferStream& operator<<(uint32_t n);
				LogBufferStream& operator<<(int64_t n);
				LogBufferStream& operator<<(uint64_t n);
				LogBufferStream& operator<<(const void* ptr);
				LogBufferStream& operator<<(float n);
				LogBufferStream& operator<<(double n);
				LogBufferStream& operator<<(char c);
				LogBufferStream& operator<<(const char* str);
				LogBufferStream& operator<<(const ::std::string& str);
				LogBufferStream& operator<<(const LogBufferStream& stream);

			private:
				char buffer_[LogBufferSize] { 0 };
				uint32_t capacity_ { LogBufferSize };
				uint32_t usedLen_ { 0 };
		};
	} // namespace base
} // namespace nets

#endif //NETS_LOGBUFFERSTREAM_H
