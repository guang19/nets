//
// Created by n021949 on 2022/5/19.
//

#include "nets/net/core/SocketChannel.h"

namespace nets::net
{
	SocketChannel::SocketChannel(EventLoopRawPtr eventLoop) : Channel(eventLoop) {}

	FdType SocketChannel::fd() const
	{
		return fd_;
	}

	void SocketChannel::handleReadEvent() {}

	void SocketChannel::handleWriteEvent() {}

	void SocketChannel::handleErrorEvent() {}
} // namespace nets::net