//
// Created by guang19
//

#include <limits>

#include "nets/net/server/ServerBootstrap.h"

using namespace nets::net;

class TestSharedServerChannelHandler : public ChannelHandler
{
public:
    void channelConnect(ChannelContext& channelContext, const InetSockAddress& localAddress,
                        const InetSockAddress& peerAddress) override
    {
        LOGS_DEBUG << "isActive=" << channelContext.isActive();
        LOGS_DEBUG << "Server channelConnect ====local address:" << localAddress.toString()
                   << " client address:" << peerAddress.toString();
//        channelContext.write("123");
//        channelContext.shutdown();
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

class TestExclusiveServerChannelHandler : public ChannelHandler
{
private:
    static int32_t SharedNum;
};
int32_t TestExclusiveServerChannelHandler::SharedNum = 0;

int main(int argc, char** argv)
{
    ServerBootstrap serverBootstrap(8);
    serverBootstrap.option(NBackLog, 1024)
        .childHandler(new TestSharedServerChannelHandler())
        .childHandler(
            [](SocketChannel& channel)
            {
//                 channel.pipeline().addLast(new TestExclusiveServerChannelHandler());
            })
        .bind(8080)
        .sync();
    return 0;
}