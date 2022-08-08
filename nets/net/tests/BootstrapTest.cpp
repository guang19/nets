//
// Created by guang19 on 2022/7/28.
//

#include <cstring>
#include <unistd.h>

#include "nets/net/core/InetSockAddress.h"
#include "nets/net/core/Socket.h"

using namespace nets::net;

int main(int argc, char** argv)
{
    FdType sockFd = socket::createTcpSocket(AF_INET);
    InetSockAddress serverAddr("127.0.0.1", 8080, false);
    socket::connect(sockFd, serverAddr.sockAddr());
    InetSockAddress clientAddr;
    SockLenType len = static_cast<SockLenType>(sizeof(SockAddr6));
    ::getsockname(sockFd, clientAddr.sockAddr(), &len);
    ::printf("client fd=%d,client addr:ip=%s,port=%d\n", sockFd, clientAddr.ip().c_str(), clientAddr.port());
    ::printf("client addr=%s\n", clientAddr.toString().c_str());
    char sendBuf[1024] = "“发送的数据”";
    size_t n1 = ::strlen(sendBuf);
    socket::write(sockFd, sendBuf, n1);
    ::shutdown(sockFd, SHUT_WR);

    char recvBuf[1024] = {0};
    size_t n2 = ::strlen(recvBuf);
    int bytes = socket::read(sockFd, recvBuf, n2);
    printf("接受的数据:%d\t%s\n", bytes, recvBuf);
    socket::closeFd(sockFd);
}

