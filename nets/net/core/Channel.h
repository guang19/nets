//
// Created by guang19 on 2022/5/15.
//

#ifndef NETS_CHANNEL_H
#define NETS_CHANNEL_H

#include <cstdint>
#include <sys/epoll.h>

#include "nets/net/core/EventLoop.h"
#include "nets/net/core/Socket.h"

namespace nets::net
{
	namespace
	{
		enum EventType
		{
			None = 0,
			ReadEvent = EPOLLIN | EPOLLPRI,
			WriteEvent = EPOLLOUT,
			ErrorEvent = EPOLLERR
		};
	}

	class Channel : base::Noncopyable, public ::std::enable_shared_from_this<Channel>
	{
	public:
		using FdType = int32_t;
		using EventLoopPtr = ::std::shared_ptr<EventLoop>;
		using ChannelPtr = ::std::shared_ptr<Channel>;

	public:
		explicit Channel(EventLoopPtr eventLoop, FdType fd);
		virtual ~Channel() = default;

	public:
		void addChannel(ChannelPtr channel);
		void updateChannel(ChannelPtr channel);
		void removeChannel(ChannelPtr channel);

	public:
		inline FdType fd() const
		{
			return socket_.sockFd();
		}

		inline EventLoopPtr eventLoop() const
		{
			return eventLoop_;
		}

	protected:
		Socket socket_ {-1};
		EventLoopPtr eventLoop_ {nullptr};
	};
} // namespace nets::net

#endif // NETS_CHANNEL_H