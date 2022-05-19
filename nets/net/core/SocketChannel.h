//
// Created by n021949 on 2022/5/19.
//

#ifndef NETS_SOCKETCHANNEL_H
#define NETS_SOCKETCHANNEL_H

#include "nets/net/core/Channel.h"

namespace nets::net
{
	class SocketChannel : public Channel
	{
	public:
		explicit SocketChannel(EventLoopPtr eventLoop);
		~SocketChannel() override = default;

	public:
		void socket(bool ipv4 = true) override;
	};
} // namespace nets::net

#endif // NETS_SOCKETCHANNEL_H
