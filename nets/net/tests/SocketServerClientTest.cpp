//
// Created by guang19
//

#include <gtest/gtest.h>

#include "nets/net/core/InetSockAddress.h"
#include "nets/net/core/Socket.h"

using namespace nets::net;

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

::int32_t main(::int32_t argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}