//
// Created by n021949 on 2022/5/13.
//

#include <gtest/gtest.h>

#include <arpa/inet.h>

#include "nets/net/core/NetUtil.h"

using namespace nets::net;

TEST(NetUtilTest, IPV4Convert)
{
	struct sockaddr_in addr4 {};
	ipPortToInet4Addr("127.0.0.1", 8080, &addr4);
	ASSERT_EQ(addr4.sin_family, AF_INET);
	ASSERT_EQ(addr4.sin_port, htons(8080));
	ASSERT_EQ(addr4.sin_addr.s_addr, ::inet_addr("127.0.0.1"));
	::printf("%u\n", ::inet_addr("127.0.0.1"));
}

TEST(NetUtilTest, IPV6Convert)
{
	struct sockaddr_in6 addr6 {};
	ipPortToInet6Addr("2a01:198:603:0:396e:4789:8e99:890f", 8080, &addr6);
	ASSERT_EQ(addr6.sin6_family, AF_INET6);
	ASSERT_EQ(addr6.sin6_port, htons(8080));
	struct in6_addr addr6_2 {};
	::inet_pton(AF_INET6, "2a01:198:603:0:396e:4789:8e99:890f", &addr6_2);
	char buf1[512] = {0};
	char buf2[512] = {0};
	::inet_ntop(AF_INET6, &addr6.sin6_addr, buf1, static_cast<SockLenType>(sizeof(buf1)));
	::inet_ntop(AF_INET6, &addr6_2, buf2, static_cast<SockLenType>(sizeof(buf1)));
	ASSERT_STREQ(buf1, buf2);
	::printf("%s\n", buf1);
	::printf("%s\n", buf2);
}

TEST(NetUtilTest, GetSockAddressIp)
{
	Ipv4Addr addr4 {};
	char ipv4[] = {"192.168.0.1"};
	ipPortToInet4Addr(ipv4, 8080, &addr4);
	char buffer1[64] = {0};
	getIpFromSockAddr(reinterpret_cast<const SockAddr*>(&addr4), buffer1, static_cast<SockLenType>(sizeof(buffer1)));
	ASSERT_STREQ(ipv4, buffer1);

	Ipv6Addr addr6 {};
	char ipv6[] = {"2a01:198:603:0:396e:4789:8e99:890f"};
	ipPortToInet6Addr(ipv6, 8080, &addr6);
	char buffer2[64] = {0};
	getIpFromSockAddr(reinterpret_cast<const SockAddr*>(&addr6), buffer2, static_cast<SockLenType>(sizeof(buffer2)));
	ASSERT_STREQ(ipv6, buffer2);
}

TEST(NetUtilTest, SockAddressToString)
{
	Ipv4Addr addr4 {};
	char ipv4[] = {"192.168.0.1"};
	ipPortToInet4Addr(ipv4, 8080, &addr4);
	char buffer1[64] = {0};
	sockAddrToString(reinterpret_cast<const SockAddr*>(&addr4), buffer1, sizeof(buffer1));
	ASSERT_STREQ(buffer1, "192.168.0.1:8080");

	Ipv6Addr addr6 {};
	char ipv6[] = {"2a01:198:603:0:396e:4789:8e99:890f"};
	ipPortToInet6Addr(ipv6, 8080, &addr6);
	char buffer2[64] = {0};
	sockAddrToString(reinterpret_cast<const SockAddr*>(&addr6), buffer2, sizeof(buffer2));
	ASSERT_STREQ(buffer2, "[2a01:198:603:0:396e:4789:8e99:890f]:8080");
}


int main(int argc, char** argv)
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}