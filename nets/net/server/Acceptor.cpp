//
// Created by guang19 on 2022/5/17.
//

#include "nets/net/server/Acceptor.h"

namespace nets::net
{
	Acceptor::Acceptor(EventLoopPtr eventLoop, const InetAddress& listenAddr)
		: Channel(::std::move(eventLoop), util::createTcpNonBlockSocket(listenAddr.ipFamily()))
	{
		Socket socket(fd_);
		socket.setAddrReuse();
		socket.setPortReuse();
		socket.bind(listenAddr);
	}

	Acceptor::Acceptor(EventLoopPtr eventLoop, const char* ip, PortType port, bool ipv4)
		: Acceptor(::std::move(eventLoop), InetAddress(ip, port, ipv4))
	{
	}

	Acceptor::~Acceptor() {}

	void Acceptor::listen()
	{
		Socket(fd_).listen();
		if (eventLoop_->hasChannel(shared_from_this()))
		{
			eventLoop_->modifyChannel(shared_from_this());
		}
		else
		{
			eventLoop_->registerChannel(shared_from_this());
		}
	}

	void Acceptor::handleReadEvent()
	{
		while (true) {}
	}

	void Acceptor::handleWriteEvent() {}

	void Acceptor::handleCloseEvent() {}

	void Acceptor::handleErrorEvent() {}
} // namespace nets::net