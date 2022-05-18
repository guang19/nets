//
// Created by guang19 on 2022/5/15.
//

#ifndef NETS_EVENTLOOP_H
#define NETS_EVENTLOOP_H

#include <atomic>
#include <memory>

#include "nets/base/Noncopyable.h"

namespace nets::net
{
	class Channel;
	class Poller;
	class PollerFactory;

	class EventLoop : base::Noncopyable, public ::std::enable_shared_from_this<EventLoop>
	{
	public:
		using ChannelPtr = ::std::shared_ptr<Channel>;
		using EventLoopPtr = ::std::shared_ptr<EventLoop>;

	public:
		EventLoop();
		~EventLoop();

	public:
		bool isInEventLoopThread() const;
		EventLoopPtr currentThreadEventLoop() const;

		void loop();
		void shutdown();

		void addChannel(ChannelPtr channel);
		void updateChannel(ChannelPtr channel);
		void removeChannel(ChannelPtr channel);
		bool hasChannel(ChannelPtr channel);

	private:
		::std::atomic_bool running_ {false};
		::std::unique_ptr<Poller> poller_ {nullptr};
		const ::pid_t currentTid_ {0};
	};
} // namespace nets::net

#endif // NETS_EVENTLOOP_H
