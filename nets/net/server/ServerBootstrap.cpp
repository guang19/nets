//
// Created by guang19 on 2022/5/18.
//

#include "nets/net/server/ServerBootstrap.h"

namespace nets::net
{
	ServerBootstrap::ServerBootstrap() : running_(false), mainLoopGroup_(nullptr), subLoopGroup_(nullptr) {}

	ServerBootstrap::~ServerBootstrap() {}

	ServerBootstrap& ServerBootstrap::group(EventLoopGroupRawPtr loopGroup)
	{
		return this->group(loopGroup, loopGroup);
	}

	ServerBootstrap& ServerBootstrap::group(EventLoopGroupRawPtr mainLoopGroup, EventLoopGroupRawPtr subLoopGroup)
	{
		if (mainLoopGroup_ != nullptr)
		{
			throw ::std::logic_error("mainLoopGroup set already");
		}
		if (subLoopGroup_ != nullptr)
		{
			throw ::std::logic_error("subLoopGroup set already");
		}
		mainLoopGroup_ = mainLoopGroup;
		subLoopGroup_ = subLoopGroup;
		return *this;
	}

	ServerBootstrap& ServerBootstrap::bind(const InetSockAddress& listenAddr)
	{

		return *this;
	}

	ServerBootstrap& ServerBootstrap::bind(const char* ip, PortType port)
	{
		return *this;
	}

	ServerBootstrap& ServerBootstrap::bind(PortType port)
	{
		return *this;
	}

	void ServerBootstrap::startUp()
	{
		if (running_)
		{
			LOGS_DEBUG << "ServerBootstrap has started";
			return;
		}
	}
} // namespace nets::net