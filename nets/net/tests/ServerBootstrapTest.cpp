//
// Created by guang19 on 2022/7/2.
//

#include "nets/net/server/ServerBootstrap.h"

using namespace nets::net;

class TestServerChannelHandler : public ChannelHandler
{
public:
    void channelConnect(ChannelContext& channelContext, const InetSockAddress& peerAddress,
                        const InetSockAddress& localAddress) override
    {
        LOGS_INFO << "client address:" << peerAddress.toString() << "====local address:" << localAddress.toString();
    }
};

int main(int argc, char** argv)
{
    ServerBootstrap serverBootstrap(1, 8);
    serverBootstrap.option(NBackLog, 1024)
        .childOption(NBackLog, 1024)
        //        .childHandler(new TestServerChannelHandler())
        .childHandler(
            [](::std::shared_ptr<SocketChannel>& channel)
            {
                channel->pipeline()->addLast(new TestServerChannelHandler());
            })
        .bind(8080)
        .launch();
    return 0;
}