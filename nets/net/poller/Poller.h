//
// Created by n021949 on 2022/5/16.
//

#ifndef NETS_POLLER_H
#define NETS_POLLER_H

#include <unordered_map>
#include <vector>

#include "nets/base/Noncopyable.h"
#include "nets/net/core/Channel.h"

namespace nets::net
{
	class Poller : nets::base::Noncopyable
	{
	public:
		using ChannelList = ::std::shared_ptr<::std::vector<ChannelPtr>>;
		using ChannelMap = ::std::unordered_map<Channel::IdType, ChannelPtr>;

	public:
		explicit Poller(EventLoopPtr eventLoop) : eventLoop_(eventLoop) {}
		virtual ~Poller() = default;

	public:
		virtual void poll(int32_t timeoutMs, ChannelList activeChannels) = 0;
		virtual void registerChannel(ChannelPtr channel) = 0;
		virtual void modifyChannel(ChannelPtr channel) = 0;
		virtual void deregisterChannel(ChannelPtr channel) = 0;
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
	public:
		static PollerPtr getPoller();
	};
}; // namespace nets::net

#endif // NETS_POLLER_H