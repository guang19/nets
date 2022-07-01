//
// Created by guang19 on 2022/5/18.
//

#ifndef NETS_NET_SERVER_BOOTSTRAP_H
#define NETS_NET_SERVER_BOOTSTRAP_H

#include "nets/net/core/EventLoopGroup.h"
#include "nets/net/core/InetSockAddress.h"

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
		ServerBootstrap& group(EventLoopGroupRawPtr group);
		ServerBootstrap& group(EventLoopGroupRawPtr mainGroup, EventLoopGroupRawPtr subGroup);
		ServerBootstrap& bind(PortType port);
		ServerBootstrap& bind(const char* ip, PortType port);
		ServerBootstrap& bind(const InetSockAddress& listenAddr);
		void launch();

	private:
		::std::atomic_bool running_ {false};
		EventLoopGroupRawPtr mainLoopGroup_ {nullptr};
		EventLoopGroupRawPtr subLoopGroup_ {nullptr};
	};
} // namespace nets::net

#endif // NETS_NET_SERVER_BOOTSTRAP_H
