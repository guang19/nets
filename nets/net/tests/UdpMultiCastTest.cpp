//
// Created by guang19
//

#include <gtest/gtest.h>

#include "nets/net/bootstrap/Bootstrap.h"

using namespace nets::net;

const ::std::string MultiCastIp = "226.1.1.1";

TEST(UdpMultiCastTest, SockRecipient1)
{
    FdType sockFd = socket::createUdpSocket();
    socket::setSockReuseAddr(sockFd, true);
    socket::setSockReusePort(sockFd, true);
    InetSockAddress localAddr(InetSockAddress::createAnySockAddress(12333));
    socket::bind(sockFd, localAddr);
    socket::addIpMemberShipByLocalAddr(sockFd, MultiCastIp, "172.30.208.26");
    char buf[64] = {0};
    while (true)
    {
        ::memset(buf, 0, sizeof(buf));
        ::recvfrom(sockFd, buf, sizeof(buf), 0, nullptr, nullptr);
        printf("UdpMultiCastTest SockRecipient1 recv message=%s\n", buf);
    }
    socket::closeFd(sockFd);
}

TEST(UdpMultiCastTest, SockRecipient2)
{
    FdType sockFd = socket::createUdpSocket();
    socket::setSockReuseAddr(sockFd, true);
    socket::setSockReusePort(sockFd, true);
    InetSockAddress localAddr(InetSockAddress::createAnySockAddress(12333));
    socket::bind(sockFd, localAddr);
    socket::addIpMemberShipByIfIndex(sockFd, MultiCastIp, "eth0");
    char buf[64] = {0};
    while (true)
    {
        ::memset(buf, 0, sizeof(buf));
        ::recvfrom(sockFd, buf, sizeof(buf), 0, nullptr, nullptr);
        printf("UdpMultiCastTest SockRecipient2 recv message=%s\n", buf);
    }
    socket::closeFd(sockFd);
}

TEST(UdpMultiCastTest, SockSender)
{

    FdType sockFd = socket::createUdpSocket();
    socket::setIpMultiCastIf(sockFd, MultiCastIp);
    InetSockAddress recipients(MultiCastIp, 12333);
    socket::sendTo(sockFd, "This is a multicast message", ::strlen("This is a multicast message"), recipients);
    socket::sendTo(sockFd, "This is a multicast message", ::strlen("This is a multicast message"), recipients);
    socket::closeFd(sockFd);
}

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
    }
};

TEST(UdpMultiCastTest, UdpMultiCastRepient1)
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

TEST(UdpMultiCastTest, UdpMultiCastRepient2)
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

class TestUdpMultiCastSenderHandler : public DatagramChannelHandler
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

TEST(UdpMultiCastTest, UdpMultiCastSender)
{
    Bootstrap()
        .option(SockOption::BROADCAST, true)
        .channelHandler(
            [](DatagramChannel& channel)
            {
                channel.pipeline().addLast(new TestUdpMultiCastSenderHandler);
            })
        .bind()
        .sync();
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}