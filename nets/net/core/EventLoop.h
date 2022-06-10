//
// Created by guang19 on 2022/5/15.
//

#ifndef NETS_EVENTLOOP_H
#define NETS_EVENTLOOP_H

#include <atomic>
#include <memory>
#include <vector>

#include "nets/base/Noncopyable.h"

namespace nets::net
{
	class Channel;
	class Poller;
	class PollerFactory;
	class EventLoop;

	class EventLoop : nets::base::Noncopyable, public ::std::enable_shared_from_this<EventLoop>
	{
	public:
		using ChannelPtr = ::std::shared_ptr<Channel>;
		using ChannelList = ::std::shared_ptr<::std::vector<ChannelPtr>>;
		using PollerPtr = ::std::unique_ptr<Poller>;
		using EventLoopPtr = ::std::shared_ptr<EventLoop>;

	public:
		EventLoop();
		~EventLoop();

	public:
		void loop();
		void shutdown();

		bool isInEventLoopThread() const;
		EventLoopPtr currentThreadEventLoop() const;

		void notify();

		void registerChannel(ChannelPtr channel);
		void modifyChannel(ChannelPtr channel);
		void deregisterChannel(ChannelPtr channel);
		bool hasChannel(ChannelPtr channel);

	private:
		::std::atomic_bool running_ {false};
		PollerPtr poller_ {nullptr};
		ChannelList activeChannels_ {nullptr};
		ChannelPtr notifier_ {nullptr};
	};
} // namespace nets::net

#endif // NETS_EVENTLOOP_H
