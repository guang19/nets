//
// Created by guang19 on 2022/5/18.
//

#ifndef NETS_SERVERSOCKETCHANNEL_H
#define NETS_SERVERSOCKETCHANNEL_H

#include "nets/net/core/Channel.h"
#include "nets/net/core/Socket.h"
#include "nets/net/core/InetSockAddress.h"

namespace nets::net
{
	class ServerSocketChannel : public Channel
	{
	public:
		explicit ServerSocketChannel(EventLoopPtr eventLoop, SockAddrFamily sockAddrFamily);
		~ServerSocketChannel() override;

	public:
		void bind(const InetSockAddress& sockAddress);

		inline FdType sockFd() const override
		{
			return sockFd_;
		}

	private:
		int32_t sockFd_ {-1};
	};
} // namespace nets::net

#endif // NETS_SERVERSOCKETCHANNEL_H
