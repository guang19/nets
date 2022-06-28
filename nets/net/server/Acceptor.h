//
// Created by guang19 on 2022/6/23.
//

#ifndef NETS_ACCEPTOR_H
#define NETS_ACCEPTOR_H

#include "nets/net/core/ChannelHandler.h"

namespace nets::net
{
	class Acceptor : public ChannelHandler
	{
	public:
		Acceptor() = default;
		~Acceptor() = default;
	};
}

#endif // NETS_ACCEPTOR_H
