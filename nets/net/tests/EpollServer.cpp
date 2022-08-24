//
// Created by guang19
//

#include <cstdint>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <vector>

#include "nets/base/Timestamp.h"
#include "nets/net/core/InetSockAddress.h"
#include "nets/net/core/Socket.h"

using namespace nets::base;
using namespace nets::net;

void testWriteV(int32_t sockFd)
{
    Timestamp start(Timestamp::now());
    const int32_t count = 5;
    const char* strArr[count] = {"hello client\n",          "this is first message\n",  "this is second message\n",
                                 "this is third message\n", "this is fourth message\n"};
    SSizeType expectedBytes = 0;
    IoVec iovecs[count];
    for (int32_t i = 0; i < count; ++i)
    {
        SizeType len = ::strlen(strArr[i]);
        iovecs[i].iov_base = const_cast<char*>(&strArr[i][0]);
        iovecs[i].iov_len = len;
        expectedBytes += len;
    }
    SSizeType bytes = socket::writev(sockFd, iovecs, count);
    Timestamp end(Timestamp::now());
    ::printf("expectedBytes=%ld, write bytes=%ld, writeV=%d\n", expectedBytes, bytes, end.microsFromTimestamp() - start.microsFromTimestamp());
}

int32_t main(int32_t argc, char** argv)
{
    FdType listenFd = socket::createTcpSocket(AF_INET);

    ::std::vector<struct epoll_event> epollEvents(20);
    InetSockAddress serverAddr = InetSockAddress::createAnySockAddress(8080);
    socket::bind(listenFd, serverAddr.sockAddr());
    socket::listen(listenFd, 1024);

    FdType epollFd = ::epoll_create1(EPOLL_CLOEXEC);
    struct epoll_event accpetEpollEvent{};
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
                int32_t optval;
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
                    FdType connFd = socket::accept(listenFd, clientAddr.sockAddr6());
                    ::printf("client fd=%d,client addr:ip=%s,port=%d\n", connFd, clientAddr.ip().c_str(), clientAddr.port());
                    ::printf("client addr=%s\n", clientAddr.toString().c_str());
                    struct epoll_event epollEvent {};
                    epollEvent.data.fd = connFd;
                    epollEvent.events = EPOLLIN;
                    epoll_ctl(epollFd, EPOLL_CTL_ADD, connFd, &epollEvent);
                }
                else
                {
                    FdType sockFd = epollEvents[i].data.fd;
                    char buf[512] = {0};
                    SSizeType n = socket::read(sockFd, buf, sizeof(buf));
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
                        ::sleep(2);
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
                testWriteV(sockFd);
                struct epoll_event epollEvent {};
                epollEvent.data.fd = sockFd;
                epollEvent.events = EPOLLIN;
                epoll_ctl(epollFd, EPOLL_CTL_MOD, sockFd, &epollEvent);
            }
        }
    }
    return 0;
}