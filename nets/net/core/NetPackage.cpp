//
// Created by YangGuang on 2022/5/8.
//

#include "nets/net/core/NetPackage.h"

namespace nets::net
{
	namespace
	{
		constexpr base::ByteBuffer::SizeType DefaultNetPackageLength = 1024;
	}

	NetPackage::NetPackage() : ByteBuffer(DefaultNetPackageLength) {}

	NetPackage::NetPackage(SizeType capacity) : ByteBuffer(capacity) {}
} // namespace nets::net
