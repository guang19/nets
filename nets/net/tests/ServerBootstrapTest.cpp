//
// Created by guang19
//

#include "nets/net/server/ServerBootstrap.h"
#include "nets/base/Timestamp.h"

using namespace nets::net;
using namespace nets::base;

class TestServerChannelHandler : public ChannelHandler
{
public:
    void channelConnect(ChannelContext& channelContext, const InetSockAddress& localAddress,
                        const InetSockAddress& peerAddress) override
    {
        LOGS_DEBUG << "isActive=" << channelContext.isActive();
        LOGS_DEBUG << "Server channelConnect ====local address:" << localAddress.toString()
                   << " client address:" << peerAddress.toString();
//        timerId_ = channelContext.channel().eventLoop()->scheduleAtFixedRate(2000, 2000, ::std::bind(&TestServerChannelHandler::testScheduleTask, this));
    }

    void channelDisconnect(ChannelContext& channelContext) override
    {
        LOGS_DEBUG << "Server channelDisconnect:" << channelContext.peerAddress().toString();
    }

    void channelRead(ChannelContext& channelContext, ByteBuffer& message) override
    {
        LOGS_DEBUG << "Server recv client message is:" << message.toString();
        channelContext.write(message);
        channelContext.write(message);
//        channelContext.write(message);
//        if (message.readInt8() == 1)
//        {
//            channelContext.channel().eventLoop()->cancelScheduleTask(timerId_);
//        }
    }

    void channelWriteComplete(ChannelContext& channelContext) override
    {
        LOGS_DEBUG << "Server channelWriteComplete";
//        channelContext.write("server channelWriteComplete");
    }

    void exceptionCaught(ChannelContext& channelContext, const std::exception& exception) override
    {
        LOGS_DEBUG << "Server exceptionCaught";
    }

private:
    void testScheduleTask()
    {
        LOGS_DEBUG << "testScheduleTask, now=" << Timestamp::now().millisecondsSinceEpoch();
    }

private:
    ::int32_t sharedNum_ = 0;
    Timer::TimerId timerId_ {};
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