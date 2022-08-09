//
// Created by guang19 on 2022/6/25.
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
    InetSockAddress clientAddr;
    SockLenType len = static_cast<SockLenType>(sizeof(SockAddr6));
    ::getsockname(sockFd, clientAddr.sockAddr(), &len);
    ::printf("client fd=%d,client addr:ip=%s,port=%d\n", sockFd, clientAddr.ip().c_str(), clientAddr.port());
    ::printf("client addr=%s\n", clientAddr.toString().c_str());
    char buf[1024] = "asdasd1233asd";
    size_t n = ::strlen(buf);
    printf("bytes = %ld", n);
    //	size_t count = n;
    while (true)
    {
        socket::write(sockFd, buf, n);
        socket::closeFd(sockFd);
        //		count += count;
        //		if (count >= 131072)
        //		{
        //			socket::write(sockFd, "131072 bytes", ::strlen("131072 bytes"));
        //			break;
        //		}
        ::sleep(5);
        break;
    }
    return 0;
}