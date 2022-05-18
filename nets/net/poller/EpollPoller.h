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
		using Event = struct epoll_event;
		using EventList = ::std::vector<Event>;

	public:
		explicit EpollPoller(EventLoopPtr eventLoop);
		~EpollPoller() override;

	public:
		void poll() override;
		void addChannel(ChannelPtr channel) override;
		void updateChannel(ChannelPtr channel) override;
		void removeChannel(ChannelPtr channel) override;

	private:
		FdType epollFd_ {0};
		// dynamically growing array of events
		EventList events_ {};
	};
}; // namespace nets::net

#endif // NETS_EPOLLSELECTOR_H
