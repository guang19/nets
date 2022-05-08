//
// Created by YangGuang on 2022/4/3.
//

#include "nets/base/log/LogBuffer.h"

namespace nets
{
	namespace base
	{

		namespace
		{
			// log length limit: 4K
			constexpr uint32_t DefaultLogBufferLength = (1024 << 2);
		}

		LogBuffer::LogBuffer() : ByteStream(DefaultLogBufferLength)
		{
		}

		LogBuffer::LogBuffer(uint32_t logBufferLength) : ByteStream(logBufferLength)
		{
		}
	} // namespace base
} // namespace nets