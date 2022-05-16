//
// Created by n021949 on 2022/5/16.
//

#ifndef NETS_SELECTOR_H
#define NETS_SELECTOR_H

#include <map>

#include "nets/net/core/Channel.h"
#include "nets/net/core/EventLoop.h"

namespace nets::net
{
	class Selector : base::Noncopyable
	{
	public:
		using FdType = int32_t;
		using EventLoopPtr = ::std::shared_ptr<EventLoop>;
		using ChannelPtr = ::std::shared_ptr<Channel>;
		using ChannelMap = ::std::map<FdType, ChannelPtr>;

	public:
		explicit Selector(EventLoopPtr eventLoop) : eventLoop_(::std::move(eventLoop)) {}
		virtual ~Selector() = default;

	public:
		virtual void select() = 0;
		virtual void addChannel(ChannelPtr channel) = 0;
		virtual void updateChannel(ChannelPtr channel) = 0;
		virtual void removeChannel(ChannelPtr channel) = 0;
		bool hasChannel(const ChannelPtr& channel);

		inline EventLoopPtr eventLoop() const
		{
			return eventLoop_;
		}

	protected:
		ChannelMap channels_ {};
		EventLoopPtr eventLoop_ {nullptr};
	};

	class SelectorFactory
	{
	};
}; // namespace nets::net

#endif // NETS_SELECTOR_H
