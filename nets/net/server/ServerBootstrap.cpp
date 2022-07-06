//
// Created by guang19 on 2022/5/18.
//

#include "nets/net/server/ServerBootstrap.h"

#include <unistd.h>

namespace nets::net
{
	namespace
	{
		constexpr char MainEventLoopGroupName[] = "MainLoopGroup";
		constexpr char SubEventLoopGroupName[] = "SubLoopGroup";
		constexpr ServerBootstrap::NType DefaultNumbOfMainEventLoops = 1;
		const ServerBootstrap::NType DefaultNumbOfSubEventLoops = ::sysconf(_SC_NPROCESSORS_ONLN) << 1;
	} // namespace

	ServerBootstrap::ServerBootstrap(NType numOfSubEventLoops) : ServerBootstrap(0, numOfSubEventLoops) {}

	ServerBootstrap::ServerBootstrap(NType numOfMainEventLoops, NType numOfSubEventLoops) : running_(false)
	{
		numOfMainEventLoops = numOfMainEventLoops <= 0 ? DefaultNumbOfMainEventLoops : numOfMainEventLoops;
		numOfSubEventLoops = numOfSubEventLoops <= 0 ? DefaultNumbOfSubEventLoops : numOfSubEventLoops;
		mainLoopGroup_ = ::std::make_unique<EventLoopGroup>(numOfMainEventLoops, MainEventLoopGroupName),
		subLoopGroup_ = ::std::make_unique<EventLoopGroup>(numOfSubEventLoops, SubEventLoopGroupName);
	}

	ServerBootstrap::~ServerBootstrap() {}

	ServerBootstrap& ServerBootstrap::channelHandler(const ChannelInitializationCallback& channelInitializationCallback)
	{
		channelInitializationCallback_ = channelInitializationCallback;
		return *this;
	}

	ServerBootstrap& ServerBootstrap::bind(const char* ip, PortType port)
	{
		bind(InetSockAddress(ip, port));
		return *this;
	}

	ServerBootstrap& ServerBootstrap::bind(PortType port)
	{
		bind(InetSockAddress::createAnySockAddress(port));
		return *this;
	}

	ServerBootstrap& ServerBootstrap::bind(const InetSockAddress& localAddress)
	{
		mainLoopGroup_->loopEach();
		mainLoopGroup_->execute(
			[&]()
			{
				auto serverSocketChannel = ::std::make_shared<ServerSocketChannel>(mainLoopGroup_->next());
				serverSocketChannel->bind(localAddress);
				serverSocketChannel->setChannelInitializationCallback(channelInitializationCallback_);
			});
		return *this;
	}

	void ServerBootstrap::launch()
	{
		if (running_)
		{
			LOGS_DEBUG << "ServerBootstrap has started";
			return;
		}
		running_ = true;
		subLoopGroup_->loopEach();
		mainLoopGroup_->syncEach();
	}
} // namespace nets::net