//
// Created by guang19
//

#include <gtest/gtest.h>

#include "nets/net/bootstrap/Bootstrap.h"

using namespace nets;

LoggerPtr testLogger = LOGGER_MGR->getLogger("UdpBroadcastTest");

class TestUdpRecipientHandler : public DatagramChannelHandler
{
public:
    void channelRead(DatagramChannelContext& channelContext, DatagramPacket& message) override
    {
        NETS_LOG_DEBUG(testLogger) << "TestUdpRecipientHandler::channelRead recv from " << message.recipient().toString()
                   << "\nmessage is:" << message.byteBuffer().toString();
    }
};

TEST(UdpBroadcastTest, UdpBroadcastRepient1)
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

TEST(UdpBroadcastTest, UdpBroadcastRepient2)
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

class TestUdpBroadcastSenderHandler : public DatagramChannelHandler
{
public:
    void channelActive(DatagramChannelContext& channelContext) override
    {
        NETS_LOG_DEBUG(testLogger) << "TestUdpRecipientHandler::channelActive";
        // broadcast address, such as 192.168.223.255
        channelContext.write("Hello UdpRecipient", InetSockAddress("xx.xx.xx.255", 8080));
    }
};

TEST(UdpBroadcastTest, UdpBroadcastSender)
{
    Bootstrap()
        .option(SockOption::BROADCAST, true)
        .channelHandler(
            [](DatagramChannel& channel)
            {
                channel.pipeline().addLast(new TestUdpBroadcastSenderHandler);
            })
        .bind()
        .sync();
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}