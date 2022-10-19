// MIT License
//
// Copyright (c) 2022 guang19
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

// @brief Test for epoll

#include <gtest/gtest.h>

#include <cstring>
#include <fcntl.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <vector>

#include "nets/base/Timestamp.h"
#include "nets/net/core/InetSockAddress.h"
#include "nets/net/core/Socket.h"

using namespace nets;

void testWriteV(Int32Type sockFd)
{
    Timestamp start(Timestamp::now());
    const Int32Type count = 5;
    const char* strArr[count] = {"hello client\n", "this is first message\n", "this is second message\n",
                                 "this is third message\n", "this is fourth message\n"};
    SSizeType expectedBytes = 0;
    IoVec iovecs[count];
    for (Int32Type i = 0; i < count; ++i)
    {
        SizeType len = ::strlen(strArr[i]);
        iovecs[i].iov_base = const_cast<char*>(&strArr[i][0]);
        iovecs[i].iov_len = len;
        expectedBytes += len;
    }
    SSizeType bytes = socket::writev(sockFd, iovecs, count);
    Timestamp end(Timestamp::now());
    ::printf("expectedBytes=%ld, write bytes=%ld, writeV=%ld\n", expectedBytes, bytes,
             end.microsPartOfTimestamp() - start.microsPartOfTimestamp());
}

TEST(EpollServerClienTest, EpollServer)
{
    FdType listenFd = socket::createTcpSocket(AF_INET);

    ::std::vector<struct epoll_event> epollEvents(20);
    InetSockAddress serverAddr = InetSockAddress::createAnySockAddress(8080);
    socket::bind(listenFd, serverAddr);
    socket::listen(listenFd, 1024);

    FdType epollFd = ::epoll_create1(EPOLL_CLOEXEC);
    using EpollEvent = struct epoll_event;
    EpollEvent accpetEpollEvent;
    accpetEpollEvent.data.fd = listenFd;
    accpetEpollEvent.events = EPOLLIN;
    epoll_ctl(epollFd, EPOLL_CTL_ADD, listenFd, &accpetEpollEvent);
    while (true)
    {
        FdType numOfReadEvents = 0;
        numOfReadEvents = epoll_wait(epollFd, &*epollEvents.begin(), epollEvents.size(), -1);
        for (Int32Type i = 0; i < numOfReadEvents; ++i)
        {
            if (epollEvents[i].events & (EPOLLHUP | EPOLLERR))
            {
                Int32Type optval;
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
                    FdType connFd = socket::accept(listenFd, clientAddr);
                    ::printf("client fd=%d,client addr:ip=%s,port=%d\n", connFd, clientAddr.ip().c_str(), clientAddr.port());
                    ::printf("client addr=%s\n", clientAddr.toString().c_str());
                    EpollEvent epollEvent;
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
                        EpollEvent epollEvent;
                        epollEvent.data.fd = sockFd;
                        epollEvent.events = EPOLLOUT;
                        epoll_ctl(epollFd, EPOLL_CTL_MOD, sockFd, &epollEvent);
                    }
                    else if (n == 0)
                    {
                        ::sleep(2);
                        ::printf("close\n");
                        socket::closeFd(sockFd);
                        EpollEvent epollEvent;
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
                EpollEvent epollEvent;
                epollEvent.data.fd = sockFd;
                epollEvent.events = EPOLLIN;
                epoll_ctl(epollFd, EPOLL_CTL_MOD, sockFd, &epollEvent);
            }
        }
    }
}

TEST(EpollServerClienTest, EpollClient)
{
    FdType sockFd = socket::createTcpSocket(AF_INET);
    InetSockAddress serverAddr("127.0.0.1", 8080);
    socket::connect(sockFd, serverAddr);
    char buf[1024] = "Hello Server";
    SizeType n = ::strlen(buf);
    socket::write(sockFd, buf, n);
    char recvBuf[1024] = {0};
    while (true)
    {
        SSizeType bytes = socket::read(sockFd, recvBuf, sizeof(recvBuf));
        if (bytes > 0)
        {
            printf("recv message bytes=%ld, content is:%s", bytes, recvBuf);
            break;
        }
    }
    socket::closeFd(sockFd);
}