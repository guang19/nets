//
// Created by YangGuang on 2022/5/8.
//

#ifndef NETS_NET_NET_PACKAGE_H
#define NETS_NET_NET_PACKAGE_H

#include "nets/base/ByteBuffer.h"

namespace nets::net
{
	class NetPackage : public nets::base::ByteBuffer
	{
	public:
		NetPackage();
		explicit NetPackage(SizeType capacity);
		~NetPackage() override = default;
	};
} // namespace nets::net

#endif // NETS_NET_NET_PACKAGE_H
