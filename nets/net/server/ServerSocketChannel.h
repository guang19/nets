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
		using InitChannelCallback = ::std::function<void (ChannelPtr channel)>;

	private:
		class Acceptor : public ChannelHandler
		{
		private:
			using InitChannelCallbackList = ::std::vector<InitChannelCallback>;

		public:
			Acceptor() = default;
			~Acceptor() = default;

		public:
			inline void addInitChannelCallback(const InitChannelCallback& initChannelCallback)
			{
				initChannelCallbacks_.push_back(initChannelCallback);
			}

		private:
			InitChannelCallbackList initChannelCallbacks_ {};
		};

	public:
		explicit ServerSocketChannel();
		~ServerSocketChannel() override;

	public:
		void bind(const InetSockAddress& sockAddress);

		inline FdType fd() const override
		{
			return sockFd_;
		}

		void handleReadEvent() override;
		void handleWriteEvent() override;
		void handleErrorEvent() override;

	private:
		FdType sockFd_ {socket::InvalidFd};
		FdType idleFd_ {socket::InvalidFd};
	};
} // namespace nets::net

#endif // NETS_NET_SERVER_SOCKET_CHANNEL_H