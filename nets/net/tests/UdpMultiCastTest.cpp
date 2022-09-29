//
// Created by guang19
//

#include <gtest/gtest.h>

#include "nets/net/bootstrap/Bootstrap.h"

using namespace nets::net;

const StringType MultiCastIp = "224.1.1.1";

TEST(UdpMultiCastTest, SockRecipient1)
{
    FdType sockFd = socket::createUdpSocket();
    socket::setSockReuseAddr(sockFd, true);
    socket::setSockReusePort(sockFd, true);
    InetSockAddress localAddr(InetSockAddress::createAnySockAddress(12333));
    socket::bind(sockFd, localAddr);
    socket::addIpMemberShipByLocalAddr(sockFd, MultiCastIp, "192.168.24.128");
    char buf[64] = {0};
    while (true)
    {
        ::memset(buf, 0, sizeof(buf));
        printf("recvFrom...\n");
        ::fflush(stdout);
        InetSockAddress sourceAddr;
        socket::recvFrom(sockFd, buf, sizeof(buf), sourceAddr);
        printf("UdpMultiCastTest SockRecipient1 recv message=%s\n", buf);
        ::fflush(stdout);
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
    socket::addIpMemberShipByIfIndex(sockFd, MultiCastIp, "ens33");
    char buf[64] = {0};
    while (true)
    {
        ::memset(buf, 0, sizeof(buf));
        printf("recvFrom...\n");
        ::fflush(stdout);
        InetSockAddress sourceAddr;
        socket::recvFrom(sockFd, buf, sizeof(buf), sourceAddr);
        printf("UdpMultiCastTest SockRecipient2 recv message=%s\n", buf);
        ::fflush(stdout);
    }
    socket::closeFd(sockFd);
}

TEST(UdpMultiCastTest, SockSender)
{

    FdType sockFd = socket::createUdpSocket();
    socket::setIpMultiCastIf(sockFd, "192.168.24.128");
    InetSockAddress recipients(MultiCastIp, 12333);
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
        .option(SockOption::IP4_MULTICAST_IF, "192.168.24.128")
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