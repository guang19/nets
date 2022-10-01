//
// Created by guang19
//

#include <gtest/gtest.h>

#include "nets/net/core/Socket.h"

using namespace nets;
using namespace nets::socket;

TEST(SocketTest, CreateSocket)
{
    FdType sockFd = -1;
    ASSERT_GE((sockFd = socket::createTcpSocket()), 0);
    socket::closeFd(sockFd);
    ASSERT_GE((sockFd = socket::createUdpSocket()), 0);
    socket::closeFd(sockFd);
}

TEST(SocketTest, KeepAlive)
{
    FdType sockFd = -1;
    ASSERT_GE((sockFd = socket::createTcpSocket()), 0);
    socket::setSockKeepAlive(sockFd, true);
    socket::setSockKeepIdle(sockFd, 10);
    socket::setSockKeepIntvl(sockFd, 5);
    socket::setSockKeepCnt(sockFd, 3);
    socket::closeFd(sockFd);
}

TEST(SocketTest, TcpNoDelay)
{
    FdType sockFd = -1;
    ASSERT_GE((sockFd = socket::createTcpSocket()), 0);
    socket::setTcpNoDelay(sockFd, true);
    socket::closeFd(sockFd);
}

TEST(SocketTest, ReuseAddr)
{
    FdType sockFd = socket::createTcpSocket(AF_INET);
    ASSERT_GE(sockFd, 0);
    OptValType optVal = 0;
    auto length = static_cast<SockLenType>(sizeof(optVal));
    ::getsockopt(sockFd, SOL_SOCKET, SO_REUSEADDR, &optVal, &length);
    ASSERT_EQ(optVal, 0);
    ::getsockopt(sockFd, SOL_SOCKET, SO_REUSEPORT, &optVal, &length);
    ASSERT_EQ(optVal, 0);
}

TEST(SocketTest, Linger)
{
    FdType sockFd = socket::createTcpSocket(AF_INET);
    ASSERT_GE(sockFd, 0);
    SockLinger linger_ {};
    auto lingerLen = static_cast<SockLenType>(sizeof(SockLinger));
    ::getsockopt(sockFd, SOL_SOCKET, SO_LINGER, &linger_, &lingerLen);
    ASSERT_EQ(linger_.l_onoff, 0);
    ASSERT_EQ(linger_.l_linger, 0);
}

TEST(SocketTest, TcpSocketSendBuf)
{
    ASSERT_EQ(socket::getTcpSockSendBuf(), 16384);
}

TEST(SocketTest, TcpSocketRecvBuf)
{
    ASSERT_EQ(socket::getTcpSockRecvBuf(), 131072);
}

TEST(SocketTest, UdpSocketSendBuf)
{
    ASSERT_EQ(socket::getUdpSockSendBuf(), 212992);
}

TEST(SocketTest, UdpSocketRecvBuf)
{
    ASSERT_EQ(socket::getUdpSockRecvBuf(), 212992);
}

TEST(SocketTest, MulticastLoop)
{
    FdType sockFd = -1;
    ASSERT_GE((sockFd = socket::createUdpSocket()), 0);
    socket::setIpMulticastLoop(sockFd, true);
    socket::closeFd(sockFd);
    ASSERT_GE((sockFd = socket::createUdpSocket(AF_INET6)), 0);
    socket::setIpv6MulticastLoop(sockFd, true);
    socket::closeFd(sockFd);
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}