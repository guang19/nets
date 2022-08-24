//
// Created by guang19
//

#include <limits>

#include "nets/net/server/ServerBootstrap.h"

using namespace nets::net;

class TestServerChannelHandler : public ChannelHandler
{
public:
    void channelConnect(ChannelContext& channelContext, const InetSockAddress& localAddress,
                        const InetSockAddress& peerAddress) override
    {
        LOGS_DEBUG << "isActive=" << channelContext.isActive();
        LOGS_DEBUG << "Server channelConnect ====local address:" << localAddress.toString()
                   << " client address:" << peerAddress.toString();
    }

    void channelDisconnect(ChannelContext& channelContext) override
    {
        LOGS_DEBUG << "Server channelDisconnect";
    }

    void channelRead(ChannelContext& channelContext, ByteBuffer& message) override
    {
        LOGS_DEBUG << "Server recv client message is:" << message.toString();
        channelContext.write(message);
    }

    void channelWriteComplete(ChannelContext& channelContext) override
    {
        LOGS_DEBUG << "Server channelWriteComplete";
    }

    void exceptionCaught(ChannelContext& channelContext, const std::exception& exception) override
    {
        LOGS_DEBUG << "Server exceptionCaught";
    }

private:
    int32_t sharedNum = 0;
};

int main(int argc, char** argv)
{
    ServerBootstrap serverBootstrap(8);
    serverBootstrap.option(NTcpSendBuffer, 1024)
        .option(NTcpRecvBuffer, 1024)
        //        .childHandler(new TestServerChannelHandler())
        .childHandler(
            [](SocketChannel& channel)
            {
                channel.pipeline().addLast(new TestServerChannelHandler());
            })
        .bind(8080)
        .sync();
    return 0;
}