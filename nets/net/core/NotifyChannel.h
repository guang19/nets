//
// Created by guang19 on 2022/6/18.
//

#ifndef NETS_NET_NOTIFY_CHANNEL_H
#define NETS_NET_NOTIFY_CHANNEL_H

#include "nets/net/core/Channel.h"

namespace nets::net
{
	class NotifyChannel : public Channel
	{
	public:
		explicit NotifyChannel(EventLoopRawPtr eventLoop);
		~NotifyChannel();

	public:
		void notify() const;
		FdType fd() const override;

	public:
		void handleReadEvent() override;
		void handleWriteEvent() override;
		void handleErrorEvent() override;

	private:
		FdType eventFd_ {socket::InvalidFd};
	};
} // namespace nets::net

#endif // NETS_NET_NOTIFY_CHANNEL_H
