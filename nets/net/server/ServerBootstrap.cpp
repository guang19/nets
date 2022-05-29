//
// Created by guang19 on 2022/5/18.
//

#include "nets/net/server/ServerBootstrap.h"

namespace nets::net
{
	ServerBootstrap& ServerBootstrap::group(EventLoopGroupRawPtr subLoops)
	{
		subLoops_.reset(subLoops);
		return *this;
	}
} // namespace nets::net