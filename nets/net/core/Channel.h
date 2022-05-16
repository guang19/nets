//
// Created by guang19 on 2022/5/15.
//

#ifndef NETS_CHANNEL_H
#define NETS_CHANNEL_H

#include "nets/net/core/EventLoop.h"

namespace nets::net
{
	class Channel : base::Noncopyable
	{
	public:
		using FdType = int32_t;
		using EventLoopPtr = ::std::shared_ptr<EventLoop>;
		using ChannelPtr = ::std::shared_ptr<Channel>;

	public:
		explicit Channel(EventLoopPtr eventLoop, FdType fd) : fd_(fd), eventLoop_(std::move(eventLoop)) {}
		virtual ~Channel() = default;

	public:
		inline FdType fd() const
		{
			return fd_;
		}

		inline EventLoopPtr eventLoop() const
		{
			return eventLoop_;
		}

	private:
		FdType fd_ {-1};
		EventLoopPtr eventLoop_ {nullptr};
	};
} // namespace nets::net

#endif // NETS_CHANNEL_H
