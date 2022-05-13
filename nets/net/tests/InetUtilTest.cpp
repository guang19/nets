//
// Created by n021949 on 2022/5/13.
//

#include <gtest/gtest.h>

#include <arpa/inet.h>

#include "nets/net/core/InetUtil.h"

using namespace nets::net;

TEST(SocketTest, IPV4Convert)
{
	struct sockaddr_in addr4 {};
	ipPortToInet4("127.0.0.1", 8080, &addr4);
	ASSERT_EQ(addr4.sin_family, AF_INET);
	ASSERT_EQ(addr4.sin_port, htons(8080));
	ASSERT_EQ(addr4.sin_addr.s_addr, ::inet_addr("127.0.0.1"));
}

TEST(SocketTest, IPV6Convert)
{

}

int main(int argc, char** argv)
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}