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

// @brief Test for socket server

#include <gtest/gtest.h>

#include "nets/net/core/InetSockAddress.h"
#include "nets/net/core/Socket.h"

using namespace nets;

TEST(SocketServerClienTest, SocketServer)
{
    FdType listenFd = socket::createTcpSocket(AF_INET);

    InetSockAddress serverAddr = InetSockAddress::createAnySockAddress(8080);
    socket::bind(listenFd, serverAddr);
    socket::listen(listenFd, 1024);

    InetSockAddress clientAddr;
    printf("accept...\n");
    ::fflush(stdout);
    FdType clientFd = socket::accept(listenFd, clientAddr);
    printf("accept fd=%d...\n", clientFd);
    ::fflush(stdout);
    char buf[1024] = {0};
    printf("read...\n");
    ::fflush(stdout);
    SSizeType ret = socket::read(clientFd, buf, sizeof(buf));
    printf("ret=%ld\n", ret);
    if (ret < 0)
    {
        printf("errno=%d\n", errno);
    }
    ::fflush(stdout);
    ::sleep(2);
    socket::closeFd(clientFd);
    socket::closeFd(listenFd);
}

TEST(SocketServerClienTest, SocketClient)
{
    FdType sockFd = socket::createTcpSocket(AF_INET);
    InetSockAddress serverAddr("127.0.0.1", 8080);
    printf("connect...\n");
    socket::connect(sockFd, serverAddr);
    printf("connect success...\n");
    ::sleep(5);
    socket::closeFd(sockFd);
}