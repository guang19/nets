//
// Created by guang19 on 2022/5/17.
//

#include "nets/net/core/Acceptor.h"

namespace nets::net
{
	Acceptor::Acceptor(EventLoopPtr eventLoop, const InetAddress& listenAddr)
		: Channel(::std::move(eventLoop), util::createTcpNonBlockSocket(listenAddr.ipFamily()))
	{
		socket_.setAddrReuse();
		socket_.setPortReuse();
		socket_.bind(listenAddr);
	}

	Acceptor::~Acceptor() {}

	void Acceptor::listen()
	{
		socket_.listen();
	}
} // namespace nets::net