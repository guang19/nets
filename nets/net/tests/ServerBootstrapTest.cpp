//
// Created by guang19 on 2022/7/2.
//

#include <limits>

#include "nets/net/server/ServerBootstrap.h"

using namespace nets::net;

class TestSharedServerChannelHandler : public ChannelHandler
{
public:
    void channelConnect(ChannelContext& channelContext, const InetSockAddress& peerAddress,
                        const InetSockAddress& localAddress) override
    {
        LOGS_INFO << "====local address:" << localAddress.toString() << "client address:" << peerAddress.toString();
    }

private:
    int32_t sharedNum = 0;
};

class TestExclusiveServerChannelHandler : public ChannelHandler
{
public:
    void channelConnect(ChannelContext& channelContext, const InetSockAddress& localAddress,
                        const InetSockAddress& peerAddress) override
    {
        LOGS_INFO << "====local address:" << localAddress.toString() << "client address:" << peerAddress.toString();
    }

private:
    static int32_t SharedNum;
};
int32_t TestExclusiveServerChannelHandler::SharedNum = 0;

int main(int argc, char** argv)
{
    ServerBootstrap serverBootstrap(1, 8);
    serverBootstrap.option(NBackLog, 1024)
        .childHandler(new TestSharedServerChannelHandler())
        .childHandler(
            [](SocketChannel& channel)
            {
                channel.pipeline().addLast(new TestExclusiveServerChannelHandler());
            })
        .bind(8080)
        .launch();
    return 0;
}