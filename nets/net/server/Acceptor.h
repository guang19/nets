//
// Created by guang19 on 2022/5/17.
//

#ifndef NETS_ACCEPTOR_H
#define NETS_ACCEPTOR_H

#include "nets/base/Noncopyable.h"
#include "nets/net/core/Channel.h"
#include "nets/net/core/InetAddress.h"

namespace nets::net
{
	class Acceptor : public Channel
	{
	public:
		using EventLoopPtr = ::std::shared_ptr<EventLoop>;

	public:
		explicit Acceptor(EventLoopPtr eventLoop, const InetAddress& listenAddr);
		explicit Acceptor(EventLoopPtr eventLoop, const char* ip, PortType port, bool ipv4 = true);
		~Acceptor();

	public:
		void listen();

	public:
		void handleReadEvent() override;
		void handleWriteEvent() override;
		void handleCloseEvent() override;
		void handleErrorEvent() override;
	};
} // namespace nets::net

#endif // NETS_ACCEPTOR_H
