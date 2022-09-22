//
// Created by guang19
//

#include "nets/net/bootstrap/Bootstrap.h"

using namespace nets::net;

class TestUdpSenderHandler : public DatagramChannelHandler
{
public:
    void channelActive(DatagramChannelContext& channelContext) override
    {
        LOGS_DEBUG << "TestUdpSenderHandler::channelActive";
    }

    void channelRead(DatagramChannelContext& channelContext, DatagramPacket& message) override
    {
        LOGS_DEBUG << "TestUdpSenderHandler::channelRead";
    }
};

int main(int argc, char** argv)
{
    Bootstrap()
        .option(NTcpRecvBuffer, 1024)
        //        .channelHandler(new TestUdpSenderHandler())
        .channelHandler(
            [](DatagramChannel& channel)
            {
                channel.pipeline().addLast(new TestUdpSenderHandler);
            })
        .bind("127.0.0.1", 8080)
        .sync();
    return 0;
}