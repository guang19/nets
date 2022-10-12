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

// @brief Test for udp multicast

#include <gtest/gtest.h>

#include "nets/net/bootstrap/Bootstrap.h"

using namespace nets;

LoggerPtr testLogger = LOGGER_MGR->getLogger("UdpMulticastTest");

const StringType gMulticastIp = "224.1.1.1";
const StringType gMulticastInterface = "ens33";
const StringType gMulticastLocalAddr = "192.168.24.128"; // my ens33 addr
const int32_t gMulticastPort = 12333;

TEST(UdpMulticastTest, SockRecipient1)
{
    FdType sockFd = socket::createUdpSocket();
    socket::setSockReuseAddr(sockFd, true);
    socket::setSockReusePort(sockFd, true);
    InetSockAddress localAddr(InetSockAddress::createAnySockAddress(gMulticastPort));
    socket::bind(sockFd, localAddr);
    socket::addIpMemberShipByIfAddr(sockFd, gMulticastIp, gMulticastLocalAddr);
    char buf[64] = {0};
    while (true)
    {
        ::memset(buf, 0, sizeof(buf));
        printf("recvFrom...\n");
        ::fflush(stdout);
        InetSockAddress sourceAddr;
        socket::recvFrom(sockFd, buf, sizeof(buf), sourceAddr);
        printf("UdpMulticastTest SockRecipient1 recv message=%s\n", buf);
        ::fflush(stdout);
    }
    socket::closeFd(sockFd);
}

TEST(UdpMulticastTest, SockRecipient2)
{
    FdType sockFd = socket::createUdpSocket();
    socket::setSockReuseAddr(sockFd, true);
    socket::setSockReusePort(sockFd, true);
    InetSockAddress localAddr(InetSockAddress::createAnySockAddress(gMulticastPort));
    socket::bind(sockFd, localAddr);
    socket::addIpMemberShipByIfIndex(sockFd, gMulticastIp, gMulticastInterface);
    char buf[64] = {0};
    while (true)
    {
        ::memset(buf, 0, sizeof(buf));
        printf("recvFrom...\n");
        ::fflush(stdout);
        InetSockAddress sourceAddr;
        socket::recvFrom(sockFd, buf, sizeof(buf), sourceAddr);
        printf("UdpMulticastTest SockRecipient2 recv message=%s\n", buf);
        ::fflush(stdout);
    }
    socket::closeFd(sockFd);
}

TEST(UdpMulticastTest, SockSender)
{

    FdType sockFd = socket::createUdpSocket();
    socket::setIpMulticastIf(sockFd, gMulticastLocalAddr);
    InetSockAddress recipients(gMulticastIp, gMulticastPort);
    socket::sendTo(sockFd, "This is a multicast message", ::strlen("This is a multicast message"), recipients);
    socket::closeFd(sockFd);
}

class TestUdpRecipientHandler : public DatagramChannelHandler
{
public:
    void channelActive(DatagramChannelContext& channelContext) override
    {
        if (channelContext.joinIpv4MulticastGroupByIfAddr(gMulticastIp, gMulticastLocalAddr))
        {
            NETS_LOG_DEBUG(testLogger) << "TestUdpRecipientHandler::channelActive joinIpv4MulticastGroupByIfAddr success";
        }
    }

    void channelRead(DatagramChannelContext& channelContext, DatagramPacket& message) override
    {
        NETS_LOG_DEBUG(testLogger) << "TestUdpRecipientHandler::channelRead recv from " << message.recipient().toString()
                   << "\nmessage is:" << message.byteBuffer().toString();
        count_++;
        if (count_ == 5)
        {
            if (channelContext.leaveIpv4MulticastGroupByIfAddr(gMulticastIp, gMulticastLocalAddr))
            {
                NETS_LOG_DEBUG(testLogger) << "TestUdpRecipientHandler::channelActive leaveIpv4MulticastGroupByIfAddr success";
            }
        }
    }

private:
    int count_ = 0;
};

TEST(UdpMulticastTest, UdpMulticastRepient1)
{
    Bootstrap()
        .option(SockOption::BROADCAST, true)
        .channelHandler(
            [](DatagramChannel& channel)
            {
                channel.pipeline().addLast(new TestUdpRecipientHandler);
            })
        .bind(gMulticastPort)
        .sync();
}

TEST(UdpMulticastTest, UdpMulticastRepient2)
{
    Bootstrap()
        .option(SockOption::BROADCAST, true)
        .channelHandler(
            [](DatagramChannel& channel)
            {
                channel.pipeline().addLast(new TestUdpRecipientHandler);
            })
        .bind(gMulticastPort)
        .sync();
}

class TestUdpMulticastSenderHandler : public DatagramChannelHandler
{
public:
    void channelActive(DatagramChannelContext& channelContext) override
    {
        NETS_LOG_DEBUG(testLogger) << "TestUdpRecipientHandler::channelActive";
        channelContext.channel().eventLoop()->scheduleAtFixedDelay(2000, 2000,
                                                                   [&]()
                                                                   {
                                                                       channelContext.write(
                                                                           ByteBuffer("this is a multicast message"),
                                                                           InetSockAddress(gMulticastIp, gMulticastPort));
                                                                   });
    }
};

TEST(UdpMulticastTest, UdpMulticastSender)
{
    Bootstrap()
        .option(SockOption::BROADCAST, true)
        .option(SockOption::IP4_MULTICAST_IF, gMulticastLocalAddr)
        .channelHandler(
            [](DatagramChannel& channel)
            {
                channel.pipeline().addLast(new TestUdpMulticastSenderHandler);
            })
        .bind()
        .sync();
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}