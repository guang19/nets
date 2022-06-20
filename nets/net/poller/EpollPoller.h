//
// Created by n021949 on 2022/5/16.
//

#ifndef NETS_NET_EPOLL_POLLER_H
#define NETS_NET_EPOLL_POLLER_H

#include <vector>

#include "nets/net/poller/Poller.h"

namespace nets::net
{
	class EpollPoller : public Poller
	{
	public:
		using ChannelRawPtr = Channel*;
		using EpollEvent = struct epoll_event;
		using EventList = ::std::vector<EpollEvent>;
		using SizeType = EventList::size_type;

	public:
		explicit EpollPoller(EventLoopPtr eventLoop);
		~EpollPoller() override;

	public:
		void poll(int32_t timeoutMs, ChannelList& activeChannels) override;
		void registerChannel(ChannelPtr channel) override;
		void modifyChannel(ChannelPtr channel) override;
		void deregisterChannel(ChannelPtr channel) override;

	private:
		void prepareChannelEvents(int32_t numOfReadyEvent, ChannelList& activeChannels);
		bool epollCtl(int32_t opt, const ChannelPtr& channel);
		const char* epollOptToString(int32_t opt);

	private:
		FdType epollFd_ {-1};
		// dynamically growing array of events
		EventList events_ {};
	};
}; // namespace nets::net

#endif // NETS_NET_EPOLL_POLLER_H
