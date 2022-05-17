//
// Created by guang19 on 2022/5/17.
//

#ifndef NETS_ACCEPTOR_H
#define NETS_ACCEPTOR_H

#include "nets/net/core/Socket.h"
#include "nets/base/Noncopyable.h"
#include "nets/net/core/EventLoop.h"

namespace nets::net
{
	class Acceptor : base::Noncopyable
	{
	public:
		using EventLoopPtr = ::std::shared_ptr<EventLoop>;

	public:
		explicit Acceptor(EventLoopPtr eventLoop, const InetAddress& listenAddr);
		~Acceptor();

	public:
		void listen();

	private:
		Socket acceptSocket_ {-1};
		EventLoopPtr eventLoop_ {nullptr};
	};
} // namespace nets::net

#endif // NETS_ACCEPTOR_H
