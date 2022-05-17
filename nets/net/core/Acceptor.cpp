//
// Created by guang19 on 2022/5/17.
//

#include "nets/net/core/Acceptor.h"

namespace nets::net
{
	Acceptor::Acceptor(EventLoopPtr eventLoop, const InetAddress& listenAddr)
		: acceptSocket_(util::createTcpNonBlockSocket(listenAddr.ipFamily())), eventLoop_(::std::move(eventLoop))
	{
		acceptSocket_.setAddrReuse();
		acceptSocket_.setPortReuse();
		acceptSocket_.bind(listenAddr);
	}

	Acceptor::~Acceptor() {}

	void Acceptor::listen()
	{
		acceptSocket_.listen();
	}
} // namespace nets::net