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

// @brief Test for udp udp unicast


#include <gtest/gtest.h>

#include "nets/net/bootstrap/Bootstrap.h"

using namespace nets;

LoggerPtr testLogger = LOGGER_MGR->getLogger("UdpUnicastTest");

class TestUdpRecipientHandler : public DatagramChannelHandler
{
public:
    void channelActive(DatagramChannelContext& channelContext) override
    {
        NETS_LOG_DEBUG(testLogger) << "TestUdpRecipientHandler::channelActive";
    }

    void channelRead(DatagramChannelContext& channelContext, DatagramPacket& message) override
    {
        NETS_LOG_DEBUG(testLogger) << "TestUdpRecipientHandler::channelRead recv from " << message.recipient().toString()
                   << "\nmessage is:" << message.byteBuffer().toString();
    }
};

TEST(UdpUnicastTest, UdpUnicastRecipient)
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
        NETS_LOG_DEBUG(testLogger) << "TestUdpSenderHandler::channelActive";
        channelContext.write("Hello UdpRecipient", InetSockAddress("127.0.0.1", 8080));
    }

    void channelRead(DatagramChannelContext& channelContext, DatagramPacket& message) override
    {
        NETS_LOG_DEBUG(testLogger) << "TestUdpSenderHandler::channelRead recv from " << message.recipient().toString()
                   << "\nmessage is:" << message.byteBuffer().toString();
        channelContext.write(message);
    }
};

TEST(UdpUnicastTest, UdpUnicastSender)
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