//
// Created by guang19 on 2022/5/18.
//

#ifndef NETS_NET_SERVER_SOCKET_CHANNEL_H
#define NETS_NET_SERVER_SOCKET_CHANNEL_H

#include <functional>

#include "nets/net/core/Channel.h"
#include "nets/net/core/InetSockAddress.h"

namespace nets::net
{
	class ServerSocketChannel : public Channel
	{
	public:
		using ChannelPtr = ::std::shared_ptr<Channel>;
		using ChannelInitializationCallback = ::std::function<void (ChannelPtr channel)>;

	private:
		class Acceptor : public ChannelHandler
		{
		public:
			Acceptor() = default;
			~Acceptor() = default;

		public:
			inline void setChannelInitializationCallback(const ChannelInitializationCallback& channelInitializationCallback)
			{
				channelInitializationCallback_ = channelInitializationCallback;
			}

		private:
			ChannelInitializationCallback channelInitializationCallback_ {nullptr};
		};

	public:
		explicit ServerSocketChannel(EventLoopRawPtr eventLoop);
		~ServerSocketChannel() override;

	public:
		inline void setChannelInitializationCallback(const ChannelInitializationCallback& channelInitializationCallback)
		{
			acceptor_->setChannelInitializationCallback(channelInitializationCallback);
		}

		inline FdType fd() const override
		{
			return sockFd_;
		}

		void bind(const InetSockAddress& sockAddress);

	private:
		FdType sockFd_ {socket::InvalidFd};
		FdType idleFd_ {socket::InvalidFd};

		using AcceptorPtr = ::std::shared_ptr<Acceptor>;
		AcceptorPtr acceptor_ {nullptr};
	};
} // namespace nets::net

#endif // NETS_NET_SERVER_SOCKET_CHANNEL_H