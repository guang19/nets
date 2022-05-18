//
// Created by n021949 on 2022/5/16.
//

#ifndef NETS_EPOLLSELECTOR_H
#define NETS_EPOLLSELECTOR_H

#include <vector>
#include <sys/epoll.h>

#include "nets/net/poller/Poller.h"

namespace nets::net
{
	class EpollPoller : public Poller
	{
	public:
		using EpollEvent = struct epoll_event;
		using EventList = ::std::vector<EpollEvent>;

	public:
		explicit EpollPoller(EventLoopPtr eventLoop);
		~EpollPoller() override;

	public:
		void poll() override;
		void registerChannel(ChannelPtr channel) override;
		void modifyChannel(ChannelPtr channel) override;
		void unregisterChannel(ChannelPtr channel) override;

	private:
		void epollCtl(int32_t opt, const ChannelPtr& channel);
		const char* epollOptToString(int32_t opt);

	private:
		FdType epollFd_ {-1};
		// dynamically growing array of events
		EventList events_ {};
	};
}; // namespace nets::net

#endif // NETS_EPOLLSELECTOR_H
