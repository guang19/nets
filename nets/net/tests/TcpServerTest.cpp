//
// Created by guang19
//

#include "nets/net/bootstrap/ServerBootstrap.h"

using namespace nets::net;
using namespace nets::base;

class TestServerChannelHandler : public SocketChannelHandler
{
public:
    void channelConnect(SocketChannelContext& channelContext, const InetSockAddress& localAddress,
                        const InetSockAddress& peerAddress) override
    {
        LOGS_DEBUG << "isActive=" << channelContext.isActive();
        LOGS_DEBUG << "Server channelConnect ====local address:" << localAddress.toString()
                   << " client address:" << peerAddress.toString();
    }

    void channelDisconnect(SocketChannelContext& channelContext) override
    {
        LOGS_DEBUG << "Server channelDisconnect:" << channelContext.peerAddress().toString();
    }

    void channelRead(SocketChannelContext& channelContext, ByteBuffer& message) override
    {
        LOGS_DEBUG << "Server recv client message is:" << message.toString();
        //        channelContext.write(message);
        channelContext.write(message,
                             [this](SocketChannelContext& ctx)
                             {
                                 writeComplete(ctx);
                             });
    }

    void writeComplete(SocketChannelContext& channelContext)
    {
        LOGS_DEBUG << "Server writeComplete";
        channelContext.write("server writeComplete");
    }
};

int main(int argc, char** argv)
{
    ServerBootstrap(8)
        .option(NTcpSendBuffer, 1024)
        .option(NTcpRecvBuffer, 1024)
        //                .childHandler(new TestServerChannelHandler())
        .childHandler(
            [](SocketChannel& channel)
            {
                channel.pipeline().addLast(new TestServerChannelHandler());
            })
        .bind(8080)
        .sync();
    return 0;
}