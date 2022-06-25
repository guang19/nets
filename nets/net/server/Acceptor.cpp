//
// Created by guang19 on 2022/6/23.
//

#include "nets/net/server/Acceptor.h"

namespace nets::net
{
	Acceptor::Acceptor(Acceptor::EventLoopRawPtr eventLoop) : ServerSocketChannel(eventLoop) {}

	void Acceptor::handleReadEvent() {}
	void Acceptor::handleWriteEvent() {}
	void Acceptor::handleErrorEvent() {}
}