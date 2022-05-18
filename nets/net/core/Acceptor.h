//
// Created by guang19 on 2022/5/17.
//

#ifndef NETS_ACCEPTOR_H
#define NETS_ACCEPTOR_H

#include "nets/base/Noncopyable.h"
#include "nets/net/core/Channel.h"

namespace nets::net
{
	class Acceptor : public Channel
	{
	public:
		using EventLoopPtr = ::std::shared_ptr<EventLoop>;

	public:
		explicit Acceptor(EventLoopPtr eventLoop, const InetAddress& listenAddr);
		~Acceptor();

	public:
		void listen();
	};
} // namespace nets::net

#endif // NETS_ACCEPTOR_H
