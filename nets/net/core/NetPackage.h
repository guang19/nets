//
// Created by YangGuang on 2022/5/8.
//

#ifndef NETS_NETPACKAGE_H
#define NETS_NETPACKAGE_H

#include "nets/base/ByteBuffer.h"

namespace nets
{
	namespace net
	{
		class NetPackage : public base::ByteBuffer
		{
			public:
				NetPackage();
				explicit NetPackage(SizeTp capacity);
				~NetPackage() override = default;
		};
	} // namespace net
} // namespace nets

#endif //NETS_NETPACKAGE_H
