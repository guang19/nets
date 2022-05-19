//
// Created by n021949 on 2022/5/16.
//

#ifndef NETS_POLLER_H
#define NETS_POLLER_H

#include <cstdint>
#include <unordered_map>

#include "nets/base/Noncopyable.h"
#include "nets/net/core/Channel.h"

namespace nets::net
{
	class EventLoop;

	class Poller : base::Noncopyable
	{
	public:
		using FdType = int32_t;
		using EventLoopPtr = ::std::shared_ptr<EventLoop>;
		using ChannelPtr = ::std::shared_ptr<Channel>;
		using ChannelMap = ::std::unordered_map<FdType, ChannelPtr>;

	public:
		explicit Poller(EventLoopPtr eventLoop) : eventLoop_(::std::move(eventLoop)) {}
		virtual ~Poller() = default;

	public:
		virtual void poll() = 0;
		virtual void registerChannel(ChannelPtr channel) = 0;
		virtual void modifyChannel(ChannelPtr channel) = 0;
		virtual void unregisterChannel(ChannelPtr channel) = 0;
		bool hasChannel(ChannelPtr channel);

		inline EventLoopPtr eventLoop() const
		{
			return eventLoop_;
		}

	protected:
		ChannelMap channels_ {};
		EventLoopPtr eventLoop_ {nullptr};
	};

	class PollerFactory
	{
		using PollerPtr = ::std::unique_ptr<Poller>;
	public:
		static PollerPtr getPoller();
	};
}; // namespace nets::net

#endif // NETS_POLLER_H
