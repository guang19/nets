//
// Created by guang19 on 2022/5/18.
//

#ifndef NETS_NET_SERVER_SOCKET_CHANNEL_H
#define NETS_NET_SERVER_SOCKET_CHANNEL_H

#include "nets/net/core/Channel.h"
#include "nets/net/core/InetSockAddress.h"

namespace nets::net
{
	class ServerSocketChannel : public Channel
	{
	public:
		explicit ServerSocketChannel(EventLoopRawPtr eventLoop);
		~ServerSocketChannel() override;

	public:
		void bind(const InetSockAddress& sockAddress);

		inline FdType sockFd() const override
		{
			return sockFd_;
		}

	private:
		FdType sockFd_ {socket::InvalidFd};
		FdType idleFd_ {socket::InvalidFd};
	};
} // namespace nets::net

#endif // NETS_NET_SERVER_SOCKET_CHANNEL_H