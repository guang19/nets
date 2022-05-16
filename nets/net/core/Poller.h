//
// Created by n021949 on 2022/5/16.
//

#ifndef NETS_POLLER_H
#define NETS_POLLER_H

#include <map>

#include "nets/net/core/Channel.h"
#include "nets/net/core/EventLoop.h"

namespace nets::net
{
	class Poller : base::Noncopyable
	{
	public:
		using FdType = int32_t;
		using EventLoopPtr = ::std::shared_ptr<EventLoop>;
		using ChannelPtr = ::std::shared_ptr<Channel>;
		using ChannelMap = ::std::map<FdType, ChannelPtr>;

		virtual void poll() = 0;
		virtual void updateChannel(ChannelPtr channel) = 0;
		virtual void removeChannel(ChannelPtr channel) = 0;

		bool hasChannel(ChannelPtr channel);

	private:
		EventLoopPtr ownerEventLoop_ {nullptr};
	};
}; // namespace nets::net

#endif // NETS_POLLER_H
