//
// Created by guang19 on 2022/5/18.
//

#ifndef NETS_SERVERBOOTSTRAP_H
#define NETS_SERVERBOOTSTRAP_H

#include "nets/net/core/InetSockAddress.h"
#include "nets/net/core/EventLoopGroup.h"

namespace nets::net
{
	class ServerBootstrap : nets::base::Noncopyable
	{
	public:
		ServerBootstrap();
		~ServerBootstrap();

	public:
		// single reactor
		ServerBootstrap& group(EventLoopGroup* worker);
		// main reactor listens to the socket, sub reactor handles io
		ServerBootstrap& group(EventLoopGroup* main, EventLoopGroup* sub);

		ServerBootstrap& bind(const InetSockAddress& listenAddr);
		ServerBootstrap& bind(const char* ip, PortType port);
		ServerBootstrap& bind(PortType port);
		void start();

	private:
		::std::atomic_bool running_ {false};
	};
} // namespace nets::net

#endif // NETS_SERVERBOOTSTRAP_H
