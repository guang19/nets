//
// Created by guang19 on 2022/6/25.
//

#include <sys/epoll.h>
#include <cstdint>
#include <vector>
#include <cstring>

#include "nets/net/core/InetSockAddress.h"
#include "nets/net/core/Socket.h"

using namespace nets::net;

int main(int argc, char** argv)
{
	FdType sockFd = socket::createTcpSocket(AF_INET);
	InetSockAddress serverAddr("172.17.242.12", 8080, false);
	socket::connect(sockFd, serverAddr.csockAddr());
	char buf[1024] = "你好，服务端。";
	socket::write(sockFd, buf, strlen(buf));
	socket::closeFd(sockFd);
	return 0;
}