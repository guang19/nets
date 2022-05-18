//
// Created by guang19 on 2022/5/19.
//

#ifndef NETS_SERVERSOCKET_H
#define NETS_SERVERSOCKET_H

#include "nets/net/core/Socket.h"
#include "nets/net/core/InetAddress.h"

namespace nets::net
{
	class ServerSocket : public Socket
	{
	public:
		explicit ServerSocket(const InetAddress& addr);
		explicit ServerSocket(const char* ip, PortType port, bool ipv4 = true);
		~ServerSocket();

	public:
		void bind();
		void listen();
		FdType accept(InetAddress* addr);

	private:
		InetAddress addr_;
	};
};

#endif // NETS_SERVERSOCKET_H
