//
// Created by YangGuang on 2022/5/12.
//

#include <gtest/gtest.h>

#include "nets/net/core/Socket.h"

using namespace nets::net;
using namespace nets::net::socket;

TEST(SocketTest, BasicUse)
{
	FdType sockFd = -1;
	ASSERT_GE((sockFd = socket::createTcpSocket(AF_INET)), 0);
	socket::closeSocket(sockFd);
	ASSERT_GE((sockFd = socket::createUdpSocket(AF_INET)), 0);
	socket::closeSocket(sockFd);
	socket::closeSocket(6);
}

TEST(SocketTest, SocketDefaultOpt)
{
	FdType sockFd1 = socket::createTcpSocket(AF_INET);
	OptValType optVal = 0;
	auto valPtr = static_cast<SockLenType>(sizeof(optVal));
	// SO_SNDBUF: default value is 16384 bytes
	::getsockopt(sockFd1, SOL_SOCKET, SO_SNDBUF, &optVal, &valPtr);
	ASSERT_EQ(optVal, 16384);
	// SO_RCVBUF: default value is 131072 bytes
	::getsockopt(sockFd1, SOL_SOCKET, SO_RCVBUF, &optVal, &valPtr);
	ASSERT_EQ(optVal, 131072);

	::getsockopt(sockFd1, SOL_SOCKET, SO_REUSEADDR, &optVal, &valPtr);
	ASSERT_EQ(optVal, 0);
	::getsockopt(sockFd1, SOL_SOCKET, SO_REUSEPORT, &optVal, &valPtr);
	ASSERT_EQ(optVal, 0);
	SockLinger linger_ {};
	auto lingerLen = static_cast<SockLenType>(sizeof(SockLinger));
	::getsockopt(sockFd1, SOL_SOCKET, SO_LINGER, &linger_, &lingerLen);
	ASSERT_EQ(linger_.l_onoff, 0);
	ASSERT_EQ(linger_.l_linger, 0);
}

int main(int argc, char** argv)
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}