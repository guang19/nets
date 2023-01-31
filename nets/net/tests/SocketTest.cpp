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

// @brief Test for socket api

#include <gtest/gtest.h>

#include "nets/net/core/Socket.h"

using namespace nets;
using namespace nets::socket;

TEST(SocketTest, CreateSocket)
{
    FdType sockFd = -1;
    EXPECT_GE((sockFd = socket::createTcpSocket()), 0);
    socket::closeFd(sockFd);
    EXPECT_GE((sockFd = socket::createUdpSocket()), 0);
    socket::closeFd(sockFd);
}

TEST(SocketTest, KeepAlive)
{
    FdType sockFd = -1;
    EXPECT_GE((sockFd = socket::createTcpSocket()), 0);
    socket::setSockKeepAlive(sockFd, true);
    socket::setSockKeepIdle(sockFd, 10);
    socket::setSockKeepIntvl(sockFd, 5);
    socket::setSockKeepCnt(sockFd, 3);
    socket::closeFd(sockFd);
}

TEST(SocketTest, TcpNoDelay)
{
    FdType sockFd = -1;
    EXPECT_GE((sockFd = socket::createTcpSocket()), 0);
    socket::setTcpNoDelay(sockFd, true);
    socket::closeFd(sockFd);
}

TEST(SocketTest, ReuseAddr)
{
    FdType sockFd = socket::createTcpSocket(AF_INET);
    EXPECT_GE(sockFd, 0);
    OptValType optVal = 0;
    auto length = static_cast<SockLenType>(sizeof(optVal));
    ::getsockopt(sockFd, SOL_SOCKET, SO_REUSEADDR, &optVal, &length);
    EXPECT_EQ(optVal, 0);
    ::getsockopt(sockFd, SOL_SOCKET, SO_REUSEPORT, &optVal, &length);
    EXPECT_EQ(optVal, 0);
}

TEST(SocketTest, Linger)
{
    FdType sockFd = socket::createTcpSocket(AF_INET);
    EXPECT_GE(sockFd, 0);
    SockLinger linger_ {};
    auto lingerLen = static_cast<SockLenType>(sizeof(SockLinger));
    ::getsockopt(sockFd, SOL_SOCKET, SO_LINGER, &linger_, &lingerLen);
    EXPECT_EQ(linger_.l_onoff, 0);
    EXPECT_EQ(linger_.l_linger, 0);
}

TEST(SocketTest, TcpSocketSendBuf)
{
    EXPECT_EQ(socket::getTcpSockSendBuf(), 16384);
}

TEST(SocketTest, TcpSocketRecvBuf)
{
    EXPECT_EQ(socket::getTcpSockRecvBuf(), 131072);
}

TEST(SocketTest, UdpSocketSendBuf)
{
    EXPECT_EQ(socket::getUdpSockSendBuf(), 212992);
}

TEST(SocketTest, UdpSocketRecvBuf)
{
    EXPECT_EQ(socket::getUdpSockRecvBuf(), 212992);
}

TEST(SocketTest, MulticastLoop)
{
    FdType sockFd = -1;
    EXPECT_GE((sockFd = socket::createUdpSocket()), 0);
    socket::setIpMulticastLoop(sockFd, true);
    socket::closeFd(sockFd);
    EXPECT_GE((sockFd = socket::createUdpSocket(AF_INET6)), 0);
    socket::setIpv6MulticastLoop(sockFd, true);
    socket::closeFd(sockFd);
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}