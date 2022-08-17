//
// Created by guang19
//

#include <cstring>
#include <unistd.h>

#include "nets/net/core/InetSockAddress.h"
#include "nets/net/core/Socket.h"

using namespace nets::net;

int main(int argc, char** argv)
{
    FdType sockFd = socket::createTcpSocket(AF_INET);
    InetSockAddress serverAddr("127.0.0.1", 8080);
    socket::connect(sockFd, serverAddr.sockAddr());
    char buf[1024] = "Hello Server";
    size_t n = ::strlen(buf);
    socket::write(sockFd, buf, n);
    char recvBuf[1024] = {0};
    while (true)
    {
        SSizeType bytes = socket::read(sockFd, recvBuf, sizeof(recvBuf));
        if (bytes > 0)
        {
            printf("recv message bytes=%ld, content is:%s", bytes, recvBuf);
            break;
        }
    }
    socket::closeFd(sockFd);
    return 0;
}