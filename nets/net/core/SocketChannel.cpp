//
// Created by n021949 on 2022/5/19.
//

#include "nets/net/core/SocketChannel.h"

namespace nets::net
{
	SocketChannel::SocketChannel(EventLoopPtr eventLoop) : Channel(::std::move(eventLoop)) {}

	void SocketChannel::socket(bool ipv4) {}
} // namespace nets::net