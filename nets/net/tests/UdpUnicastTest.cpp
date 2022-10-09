//
// Created by guang19
//

#include <gtest/gtest.h>

#include "nets/net/bootstrap/Bootstrap.h"

using namespace nets;

class TestUdpRecipientHandler : public DatagramChannelHandler
{
public:
    void channelActive(DatagramChannelContext& channelContext) override
    {
        NETS_SYSTEM_LOG_DEBUG << "TestUdpRecipientHandler::channelActive";
    }

    void channelRead(DatagramChannelContext& channelContext, DatagramPacket& message) override
    {
        NETS_SYSTEM_LOG_DEBUG << "TestUdpRecipientHandler::channelRead recv from " << message.recipient().toString()
                   << "\nmessage is:" << message.byteBuffer().toString();
    }
};

TEST(UdpUnicastRecipientTest, UdpUnicastRecipient)
{
    Bootstrap()
        .option(SockOption::BROADCAST, true)
        .channelHandler(
            [](DatagramChannel& channel)
            {
                channel.pipeline().addLast(new TestUdpRecipientHandler);
            })
        .bind(8080)
        .sync();
}

class TestUdpSenderHandler : public DatagramChannelHandler
{
public:
    void channelActive(DatagramChannelContext& channelContext) override
    {
        NETS_SYSTEM_LOG_DEBUG << "TestUdpSenderHandler::channelActive";
        channelContext.write("Hello UdpRecipient", InetSockAddress("127.0.0.1", 8080));
    }

    void channelRead(DatagramChannelContext& channelContext, DatagramPacket& message) override
    {
        NETS_SYSTEM_LOG_DEBUG << "TestUdpSenderHandler::channelRead recv from " << message.recipient().toString()
                   << "\nmessage is:" << message.byteBuffer().toString();
        channelContext.write(message);
    }
};

TEST(UdpUnicastSenderTest, UdpUnicastSender)
{
    Bootstrap()
        .option(SockOption::BROADCAST, true)
        .channelHandler(
            [](DatagramChannel& channel)
            {
                channel.pipeline().addLast(new TestUdpSenderHandler);
            })
        .bind()
        .sync();
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}