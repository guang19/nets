// MIT License
//
// Copyright (c) 2022 guang19
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

// @brief Test for udp broadcast

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
                channel.pipeline().addLast(::std::shared_ptr<DatagramChannelHandler>(new TestUdpRecipientHandler()));
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
                channel.pipeline().addLast(::std::shared_ptr<DatagramChannelHandler>(new TestUdpRecipientHandler()));
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
                channel.pipeline().addLast(::std::shared_ptr<DatagramChannelHandler>(new TestUdpBroadcastSenderHandler()));
            })
        .bind()
        .sync();
}