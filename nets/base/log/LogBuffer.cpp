//
// Created by YangGuang on 2022/4/3.
//

#include "nets/base/log/LogBuffer.h"

namespace nets::base
{
	namespace
	{
		// log length limit: 2K
		constexpr uint32_t DefaultLogBufferLength = (1024 << 1);
	} // namespace

	LogBuffer::LogBuffer() : ByteStream(DefaultLogBufferLength) {}

	LogBuffer::LogBuffer(uint32_t logBufferLength) : ByteStream(logBufferLength) {}

	LogBuffer::LogBuffer(const LogBuffer& logBuffer) : ByteStream(logBuffer) {}

	LogBuffer::LogBuffer(LogBuffer&& logBuffer) noexcept : ByteStream(::std::move(logBuffer)) {}

	LogBuffer& LogBuffer::operator=(const LogBuffer& logBuffer)
	{
		ByteStream::operator=(logBuffer);
		return *this;
	}

	LogBuffer& LogBuffer::operator=(LogBuffer&& logBuffer) noexcept
	{
		ByteStream::operator=(logBuffer);
		return *this;
	}
} // namespace nets::base