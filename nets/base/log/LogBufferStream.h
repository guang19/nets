//
// Created by YangGuang on 2022/4/3.
//

#ifndef NETS_LOGBUFFERSTREAM_H
#define NETS_LOGBUFFERSTREAM_H

#include <string>

#include "nets/base/StackBuffer.h"

namespace nets::base
{
	namespace
	{
		// log length limit: 2K
		constexpr uint32_t DefaultLogBufferLength = (1024 << 1);
	} // namespace

	using LogBuffer = StackBuffer<DefaultLogBufferLength>;

	class LogBufferStream : Noncopyable
	{
	public:
		LogBufferStream() = default;
		~LogBufferStream() = default;

	public:
		inline const LogBuffer& cbuffer() const
		{
			return buffer_;
		}

	public:
		LogBufferStream& operator<<(char c);
		LogBufferStream& operator<<(int8_t n);
		LogBufferStream& operator<<(uint8_t n);
		LogBufferStream& operator<<(int16_t n);
		LogBufferStream& operator<<(uint16_t n);
		LogBufferStream& operator<<(int32_t n);
		LogBufferStream& operator<<(uint32_t n);
		LogBufferStream& operator<<(int64_t n);
		LogBufferStream& operator<<(uint64_t n);
		LogBufferStream& operator<<(const void* ptr);
		LogBufferStream& operator<<(float f);
		LogBufferStream& operator<<(double f);
		LogBufferStream& operator<<(const char* str);
		LogBufferStream& operator<<(const ::std::string& str);
		LogBufferStream& operator<<(const ::std::string_view& str);
		LogBufferStream& operator<<(const LogBufferStream& stream);

	private:
		LogBuffer buffer_ {};
	};
} // namespace nets::base

#endif // NETS_LOGBUFFERSTREAM_H
