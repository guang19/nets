//
// Created by guang19
//

#include "nets/base/Timestamp.h"
#include "nets/net/server/ServerBootstrap.h"

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
        timerId_ = channelContext.channel().eventLoop()->scheduleAtFixedRate(
            2000, 2000, ::std::bind(&TestServerChannelHandler::testScheduleTask, this));
    }

    void channelDisconnect(SocketChannelContext& channelContext) override
    {
        LOGS_DEBUG << "Server channelDisconnect:" << channelContext.peerAddress().toString();
    }

    void channelRead(SocketChannelContext& channelContext, ByteBuffer& message) override
    {
        if (message.readInt8() == 1)
        {
            channelContext.channel().eventLoop()->cancelScheduleTask(timerId_);
        }
    }

private:
    void testScheduleTask()
    {
        LOGS_DEBUG << "testScheduleTask, now=" << Timestamp::now().millisecondsSinceEpoch();
    }

private:
    Timer::TimerId timerId_ {};
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
