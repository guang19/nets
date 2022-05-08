//
// Created by YangGuang on 2022/4/3.
//

#ifndef NETS_LOGBUFFER_H
#define NETS_LOGBUFFER_H

#include "nets/base/ByteStream.h"
#include "nets/base/Noncopyable.h"

namespace nets
{
	namespace base
	{
		class LogBuffer : public ByteStream
		{
			public:
				LogBuffer();
				explicit LogBuffer(SizeTp logBufferLength);
				~LogBuffer() override = default;

			public:
				const char* buffer() const
				{
					return buffer_;
				}
		};
	} // namespace base
} // namespace nets

#endif //NETS_LOGBUFFER_H
