//
// Created by n021949 on 2022/4/2.
//

#include "base/log/LogBuffer.h"

namespace nets
{
	namespace base
	{
		bool LogBuffer::append(const char *data, uint32_t len)
		{
			if (available() > len)
			{
				memcpy(buffer_ + usedLen_, data, len);
				usedLen_ += len;
				return true;
			}
			return false;
		}

	} // namespace base
} // namespace nets
