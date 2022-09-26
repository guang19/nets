//
// Created by guang19
//

#include <gtest/gtest.h>

#include "nets/net/bootstrap/Bootstrap.h"

using namespace nets::net;

class TestUdpBroadCastRecipientHandler : public DatagramChannelHandler
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
    }
};

TEST(UdpBroadCastTest, UdpBroadCastRepient1)
{
    Bootstrap()
        .option(SO_BroadCast, true)
        .channelHandler(
            [](DatagramChannel& channel)
            {
                channel.pipeline().addLast(new TestUdpBroadCastRecipientHandler);
            })
        .bind(8080)
        .sync();
}

TEST(UdpBroadCastTest, UdpBroadCastRepient2)
{
    Bootstrap()
        .option(SO_BroadCast, true)
        .channelHandler(
            [](DatagramChannel& channel)
            {
                channel.pipeline().addLast(new TestUdpBroadCastRecipientHandler);
            })
        .bind(8080)
        .sync();
}

class TestUdpBroadCastSenderHandler : public DatagramChannelHandler
{
public:
    void channelActive(DatagramChannelContext& channelContext) override
    {
        LOGS_DEBUG << "TestUdpRecipientHandler::channelActive";
        // broadcast address, such as 192.168.223.255
        channelContext.write("Hello UdpRecipient", InetSockAddress("xx.xx.xx.255", 8080));
    }

    void channelRead(DatagramChannelContext& channelContext, DatagramPacket& message) override
    {
        LOGS_DEBUG << "TestUdpRecipientHandler::channelRead recv from " << message.recipient().toString()
                   << "\nmessage is:" << message.byteBuffer().toString();
        channelContext.write(message);
    }
};

TEST(UdpBroadCastTest, UdpBroadCastSender)
{
    Bootstrap()
        .option(SO_BroadCast, true)
        .channelHandler(
            [](DatagramChannel& channel)
            {
                channel.pipeline().addLast(new TestUdpBroadCastSenderHandler);
            })
        .bind()
        .sync();
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}