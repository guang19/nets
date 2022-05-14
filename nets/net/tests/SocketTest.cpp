//
// Created by YangGuang on 2022/5/12.
//

#include <gtest/gtest.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include "nets/net/core/Socket.h"

using namespace nets::net;

TEST(InetAddressTest, BasicUse)
{
	InetAddress inetAddress1("127.0.0.1", 8080);
	ASSERT_EQ(inetAddress1.ipFamily(), AF_INET);
	ASSERT_STREQ(inetAddress1.ip().c_str(), "127.0.0.1");
	ASSERT_EQ(inetAddress1.port(), 8080);
	ASSERT_STREQ(inetAddress1.toString().c_str(), "127.0.0.1:8080");

	InetAddress inetAddress2("2a01:198:603:0:396e:4789:8e99:890f", 8080, false);
	ASSERT_EQ(inetAddress2.ipFamily(), AF_INET6);
	ASSERT_STREQ(inetAddress2.ip().c_str(), "2a01:198:603:0:396e:4789:8e99:890f");
	ASSERT_EQ(inetAddress2.port(), 8080);
	ASSERT_STREQ(inetAddress2.toString().c_str(), "[2a01:198:603:0:396e:4789:8e99:890f]:8080");
}

TEST(SocketTest, Server)
{
	int32_t socketFd = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	::printf("%lu\t%lu\t%lu\n", sizeof(struct sockaddr), sizeof(struct sockaddr_in6), sizeof(struct sockaddr_in));

	struct sockaddr_in serverAddr
	{
	};
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htobe16(8080);
	serverAddr.sin_addr.s_addr = htobe16(INADDR_ANY);
	int32_t ret = -1;
	if (0 == (ret = ::bind(socketFd, (struct sockaddr*) &serverAddr, sizeof(sockaddr))))
	{
		::printf("server socket bind success\n");
		::fflush(stdout);
		if (0 == (ret = ::listen(socketFd, 24)))
		{
			::printf("server socket start listen\n");
			::fflush(stdout);
			struct sockaddr_in clientAddr
			{
			};
			uint32_t len = sizeof(clientAddr);
			::printf("server socket start accept...\n");
			::fflush(stdout);
			while (true)
			{
				int32_t connectFd = ::accept(socketFd, (struct sockaddr*) &clientAddr, &len);
				char buffer[256] = {0};
				int32_t nbytes = 0;
				while (true)
				{
					if ((nbytes = ::recv(connectFd, buffer, sizeof(buffer), 0)) > 0)
					{
						::fprintf(stdout, "server recv data: %s\n", buffer);
						::fflush(stdout);
					}
					else
					{
						::close(connectFd);
						break;
					}
				}
			}
		}
		else
		{
			::fprintf(stderr, "server socket listen failed\n");
			::fflush(stderr);
		}
	}
	else
	{
		::fprintf(stderr, "server socket bind failed\n");
		::fflush(stderr);
	}
}

TEST(SocketTest, Client)
{
	int32_t socketFd = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	struct sockaddr_in serverAddr {};
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htobe16(8080);
	serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	int32_t ret = 0;
	if (0 == (ret = ::connect(socketFd, (struct sockaddr*) &serverAddr, sizeof(serverAddr))))
	{
		::printf("%s", "connect server success\n");
		::fflush(stdout);
		char buffer[256] = "i love you";
		do
		{
			ret = ::send(socketFd, buffer, strlen(buffer), 0);
		} while (ret > 0);
		close(socketFd);
	}
	else
	{
		::fprintf(stderr, "client socket connect failed\n");
		::fflush(stderr);
	}
}

int main(int argc, char** argv)
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}