//
// Created by guang19 on 2022/7/28.
//

#include "nets/net/client/Bootstrap.h"

using namespace nets::net;

class TestSharedClientChannelHandler : public ChannelHandler
{
public:
    void channelConnect(ChannelContext& channelContext, const InetSockAddress& localAddress,
                        const InetSockAddress& peerAddress) override
    {
        LOGS_DEBUG << "Client channelConnect ====local address:" << localAddress.toString()
                   << " server address:" << peerAddress.toString();
    }

    void channelDisconnect(ChannelContext& channelContext) override
    {
        int bytes = socket::write(channelContext.channel()->fd(), "Hello Server", strlen("Hello Server"));
        LOGS_DEBUG << bytes << " " << errno;
        LOGS_DEBUG << "Client channelDisconnect";
    }

    void channelRead(ChannelContext& channelContext, ByteBuffer& message) override
    {
        LOGS_DEBUG << "Client recv server message is:" << message.toString();
    }

    void exceptionCaught(ChannelContext& channelContext, const std::exception& exception) override
    {
        LOGS_DEBUG << "Client exceptionCaught";
    }

private:
    int32_t sharedNum = 0;
};

int main(int argc, char** argv)
{
    Bootstrap bootstrap;
    bootstrap.option(NTcpSendBuf, 20480)
        .channelHandler(new TestSharedClientChannelHandler())
        .connect("127.0.0.1", 8080)
        .sync();
//        FdType sockFd = socket::createTcpSocket(AF_INET);
//        InetSockAddress serverAddr("127.0.0.1", 8080, false);
//        socket::connect(sockFd, serverAddr.sockAddr());
//        InetSockAddress clientAddr;
//        SockLenType len = static_cast<SockLenType>(sizeof(SockAddr6));
//        ::getsockname(sockFd, clientAddr.sockAddr(), &len);
//        ::printf("client fd=%d,client addr:ip=%s,port=%d\n", sockFd, clientAddr.ip().c_str(), clientAddr.port());
//        ::printf("client addr=%s\n", clientAddr.toString().c_str());
//        while(1) {}
//        char sendBuf[1024] = "“发送的数据”";
//        size_t n1 = ::strlen(sendBuf);
//        socket::write(sockFd, sendBuf, n1);
//        ::shutdown(sockFd, SHUT_WR);
//        char recvBuf[1024] = {0};
//        int bytes = socket::read(sockFd, recvBuf, sizeof(recvBuf));
//        if (bytes > 0)
//        {
//            printf("接受的数据:%d\t%s\n", bytes, recvBuf);
//        }
//        else if (bytes == 0)
//        {
//            socket::closeFd(sockFd);
//        }
//        else
//        {
//            printf("error");
//        }
}
