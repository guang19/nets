//
// Created by guang19 on 2022/6/23.
//

#ifndef NETS_ACCEPTOR_H
#define NETS_ACCEPTOR_H

#include "nets/net/server/ServerSocketChannel.h"

namespace nets::net
{
	class Acceptor
	{
	public:
		using EventLoopRawPtr = EventLoop*;
	public:
		explicit Acceptor(EventLoopRawPtr eventLoop);
		~Acceptor() = default;

	private:
		ServerSocketChannel serverSocketChannel {nullptr};
	};
}

#endif // NETS_ACCEPTOR_H
