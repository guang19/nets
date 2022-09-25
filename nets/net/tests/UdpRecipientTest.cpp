//
// Created by guang19
//

#include "nets/net/bootstrap/Bootstrap.h"

using namespace nets::net;

class TestUdpRecipientHandler : public DatagramChannelHandler
{
public:
    void channelActive(DatagramChannelContext& channelContext) override
    {
        LOGS_DEBUG << "TestUdpRecipientHandler::channelActive";
    }

    void channelRead(DatagramChannelContext& channelContext, DatagramPacket& message) override
    {
        LOGS_DEBUG << "TestUdpRecipientHandler::channelRead recv from " << message.recipient().toString()
                   << "\nmessage is:" << message.byteBuffer().toString();
        channelContext.write(message);
    }
};

int main(int argc, char** argv)
{
    Bootstrap()
        .option(SO_UdpRecvBuffer, 1024)
        .option(SO_BroadCast, true)
        .channelHandler(
            [](DatagramChannel& channel)
            {
                channel.pipeline().addLast(new TestUdpRecipientHandler);
            })
        .bind("127.0.0.1", 8080)
        .sync();
    return 0;
}