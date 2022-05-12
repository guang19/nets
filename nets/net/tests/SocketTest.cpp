//
// Created by YangGuang on 2022/5/12.
//

#include <gtest/gtest.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

TEST(SocketTest, Server)
{
	int32_t socketFd = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	struct sockaddr_in serverAddr {};
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(8080);
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	int32_t ret = -1;
	if ((ret = ::bind(socketFd, (struct sockaddr*)&serverAddr, sizeof(sockaddr))) == 0)
	{
		::printf("server socket bind success\n");
		::fflush(::stdout);
		if ((ret = ::listen(socketFd, 24)) == 0)
		{
			::printf("server socket start listen\n");
			::fflush(::stdout);
			struct sockaddr_in clientAddr {};
			uint32_t len = sizeof(clientAddr);
			::printf("server socket start accept...\n");
			::fflush(::stdout);
			while (true)
			{
				int32_t connectFd = ::accept(socketFd, (struct sockaddr*)&clientAddr, &len);
				char buffer[256] = {0};
				int32_t nbytes = 0;
				while (true)
				{
					if ((nbytes = ::recv(connectFd, buffer, sizeof(buffer), 0)) > 0)
					{
						::fprintf(::stdout, "server recv data: %s\n", buffer);
						::fflush(::stdout);
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
			::fprintf(::stderr, "server socket listen failed\n");
			::fflush(::stderr);
		}
	}
	else
	{
		::fprintf(::stderr, "server socket bind failed\n");
		::fflush(::stderr);
	}
}

TEST(SocketTest, Client)
{
	int32_t socketFd = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	struct sockaddr_in serverAddr {};
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(8080);
	serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	int32_t ret = 0;
	if ((ret = ::connect(socketFd, (struct sockaddr*)&serverAddr, sizeof(serverAddr))) == 0)
	{
		::printf("%s", "connect server success\n");
		::fflush(::stdout);
		char buffer[256] = "i love you";
		do
		{
			ret = ::send(socketFd, buffer, strlen(buffer), 0);
		} while (ret > 0);
		close(socketFd);
	}
	else
	{
		::fprintf(::stderr, "client socket connect failed\n");
		::fflush(::stderr);
	}
}


int main(int argc, char** argv)
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}