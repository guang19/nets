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
	ipPortToInet4Address("127.0.0.1", 8080, &addr4);
	ASSERT_EQ(addr4.sin_family, AF_INET);
	ASSERT_EQ(addr4.sin_port, htons(8080));
	ASSERT_EQ(addr4.sin_addr.s_addr, ::inet_addr("127.0.0.1"));
	::printf("%u\n", ::inet_addr("127.0.0.1"));
}

TEST(SocketTest, IPV6Convert)
{
	struct sockaddr_in6 addr6 {};
	ipPortToInet6Address("2a01:198:603:0:396e:4789:8e99:890f", 8080, &addr6);
	ASSERT_EQ(addr6.sin6_family, AF_INET6);
	ASSERT_EQ(addr6.sin6_port, htons(8080));
	struct in6_addr addr6_2 {};
	::inet_pton(AF_INET6, "2a01:198:603:0:396e:4789:8e99:890f", &addr6_2);
	char buf1[512] = {0};
	char buf2[512] = {0};
	::inet_ntop(AF_INET6, &addr6.sin6_addr, buf1, sizeof(buf1));
	::inet_ntop(AF_INET6, &addr6_2, buf2, sizeof(buf1));
	ASSERT_STREQ(buf1, buf2);
	::printf("%s\n", buf1);
	::printf("%s\n", buf2);
}

int main(int argc, char** argv)
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}