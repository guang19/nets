//
// Created by YangGuang on 2022/5/8.
//

#include "nets/net/core/NetPackage.h"

namespace nets
{
	namespace net
	{
		namespace
		{
			constexpr uint32_t DefaultNetPackageLength = 512;
		}

		NetPackage::NetPackage() : ByteBuffer(DefaultNetPackageLength)
		{
		}

		NetPackage::NetPackage(ByteBuffer::SizeTp capacity) : ByteBuffer(capacity)
		{
		}
	} // namespace net
} // namespace nets