//
// Created by YangGuang on 2022/4/3.
//

#ifndef NETS_LOGBUFFER_H
#define NETS_LOGBUFFER_H

#include "nets/base/ByteStream.h"
#include "nets/base/Noncopyable.h"

namespace nets::base
{
	class LogBuffer : public ByteStream
	{
	public:
		LogBuffer();
		explicit LogBuffer(SizeType logBufferLength);
		~LogBuffer() override = default;

		LogBuffer(const LogBuffer& logBuffer);
		LogBuffer(LogBuffer&& logBuffer) noexcept;
		LogBuffer& operator=(const LogBuffer& logBuffer);
		LogBuffer& operator=(LogBuffer&& logBuffer) noexcept;

	public:
		const char* buffer() const
		{
			return buffer_.get() + readerIndex_;
		}

		SizeType len() const
		{
			return readableBytes();
		}
	};
} // namespace nets::base

#endif // NETS_LOGBUFFER_H
