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
	class Selector;
	class SelectorFactory;

	class EventLoop : base::Noncopyable
	{
	public:
		using ChannelPtr = ::std::shared_ptr<Channel>;
		using EventLoopPtr = ::std::shared_ptr<EventLoop>;

	public:
		EventLoop();
		~EventLoop();

	public:
		void loop();
		void shutdown();

		void addChannel(ChannelPtr channel);
		void updateChannel(ChannelPtr channel);
		void removeChannel(ChannelPtr channel);

	private:
		::std::atomic_bool running_ {false};
		::std::shared_ptr<Selector> selector_ {nullptr};
	};
} // namespace nets::net

#endif // NETS_EVENTLOOP_H
