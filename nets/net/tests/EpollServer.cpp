//
// Created by guang19 on 2022/6/25.
//

#include <sys/epoll.h>
#include <fcntl.h>
#include <cstdint>
#include <vector>
#include <cstring>

#include "nets/net/core/InetSockAddress.h"
#include "nets/net/core/Socket.h"

using namespace nets::net;

int main(int argc, char** argv)
{
	FdType listenFd = socket::createTcpSocket(AF_INET6);

	::std::vector<struct epoll_event> epollEvents(20);

	InetSockAddress serverAddr = InetSockAddress::createAnySockAddress(8080, true);
	socket::bind(listenFd, serverAddr.csockAddr());
	socket::listen(listenFd);

	FdType epollFd = ::epoll_create1(EPOLL_CLOEXEC);
	struct epoll_event accpetEpollEvent {};
	accpetEpollEvent.data.fd = listenFd;
	accpetEpollEvent.events = EPOLLIN;
	epoll_ctl(epollFd, EPOLL_CTL_ADD, listenFd, &accpetEpollEvent);
	while (true)
	{
		FdType numOfReadEvents = 0;
		numOfReadEvents = epoll_wait(epollFd, &*epollEvents.begin(), epollEvents.size(), -1);
		for (int32_t i = 0; i < numOfReadEvents; ++i)
		{
			if (epollEvents[i].events & (EPOLLHUP | EPOLLERR))
			{
				int optval;
				auto optlen = static_cast<SockLenType>(sizeof(optval));
				::getsockopt(epollEvents[i].data.fd, SOL_SOCKET, SO_ERROR, &optval, &optlen);
				// broken pipe
				::printf("error:%d\n", optval);
			}
			if (epollEvents[i].events & (EPOLLIN))
			{
				if (epollEvents[i].data.fd == listenFd)
				{
					::printf("listen listenFd\n");
					InetSockAddress clientAddr;
					FdType connFd = socket::acceptAddr4(listenFd, clientAddr.sockAddr());
					::printf("client fd=%d,client addr:ip=%s,port=%d\n", connFd, clientAddr.ip().c_str(), clientAddr.port());
					struct epoll_event epollEvent {};
					epollEvent.data.fd = connFd;
					epollEvent.events = EPOLLIN;
					epoll_ctl(epollFd, EPOLL_CTL_ADD, connFd, &epollEvent);
				}
				else
				{
					FdType sockFd = epollEvents[i].data.fd;
					char buf[512] = {0};
					::ssize_t n = socket::read(sockFd, buf, sizeof(buf));
					if (n > 0)
					{
						::printf("recv client msg:%s\n", buf);
						struct epoll_event epollEvent {};
						epollEvent.data.fd = sockFd;
						epollEvent.events = EPOLLOUT;
						epoll_ctl(epollFd, EPOLL_CTL_MOD, sockFd, &epollEvent);
					}
					else if (n == 0)
					{
						::printf("close\n");
						socket::closeFd(sockFd);
						struct epoll_event epollEvent {};
						epollEvent.data.fd = sockFd;
						epollEvent.events = 0;
						epoll_ctl(epollFd, EPOLL_CTL_DEL, sockFd, &epollEvent);
					}
					else
					{
						// error
					}
				}
			}
			if (epollEvents[i].events & EPOLLOUT)
			{
				FdType sockFd = epollEvents[i].data.fd;
				::printf("EPOLLOUT\n");
				socket::write(sockFd, "啦啦啦啦", strlen("啦啦啦啦"));
				struct epoll_event epollEvent {};
				epollEvent.data.fd = sockFd;
				epollEvent.events = EPOLLIN;
				epoll_ctl(epollFd, EPOLL_CTL_MOD, sockFd, &epollEvent);
			}
		}
	}
	return 0;
}