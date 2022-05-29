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
		using EventLoopGroupRawPtr = EventLoopGroup*;
		using EventLoopGroupPtr = ::std::unique_ptr<EventLoopGroup>;

	public:
		ServerBootstrap();
		~ServerBootstrap();

	public:
		ServerBootstrap& group(EventLoopGroupRawPtr subLoops);
		ServerBootstrap& bind(const InetSockAddress& listenAddr);
		ServerBootstrap& bind(const char* ip, PortType port);
		ServerBootstrap& bind(PortType port);
		void start();

	private:
		::std::atomic_bool running_ {false};
		EventLoop mainLoop_ {};
		EventLoopGroupPtr subLoops_ {};
	};
} // namespace nets::net

#endif // NETS_SERVERBOOTSTRAP_H
