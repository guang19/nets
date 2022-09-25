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
        channelContext.write("Hello UdpRecipient", InetSockAddress("127.0.0.1", 8080));
    }

    void channelRead(DatagramChannelContext& channelContext, DatagramPacket& message) override
    {
        LOGS_DEBUG << "TestUdpSenderHandler::channelRead recv from " << message.recipient().toString()
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
                channel.pipeline().addLast(new TestUdpSenderHandler);
            })
        .bind()
        .sync();
    return 0;
}