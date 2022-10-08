//
// Created by guang19
//

#include <gtest/gtest.h>

#include "nets/net/bootstrap/Bootstrap.h"
#include "nets/net/bootstrap/ServerBootstrap.h"

using namespace nets;

class TestServerChannelHandler : public SocketChannelHandler
{
public:
    void channelConnect(SocketChannelContext& channelContext, const InetSockAddress& localAddress,
                        const InetSockAddress& peerAddress) override
    {
        LOGS_DEBUG << "isActive=" << channelContext.isActive();
        LOGS_DEBUG << "Server channelConnect ====local address:" << localAddress.toString()
                   << " client address:" << peerAddress.toString();
        timerId_ = channelContext.channel().eventLoop()->scheduleAtFixedRate(
            1000, 2000,
            [&]()
            {
                LOGS_DEBUG << "TestServerChannelHandler schedule task,isActive=" << channelContext.isActive();
            });
    }

    void channelDisconnect(SocketChannelContext& channelContext) override
    {
        LOGS_DEBUG << "Server channelDisconnect:" << channelContext.peerAddress().toString();
        // if channel has destroyed,but task still running,server will throw segmentation fault exception
        //        channelContext.channel().eventLoop()->cancelScheduleTask(timerId_);
    }

    void channelRead(SocketChannelContext& channelContext, ByteBuffer& message) override
    {
        LOGS_DEBUG << "Server recv client message is:" << message.toString();
    }

private:
    TimerId timerId_ {};
};

TEST(ScheduleTaskTest, TcpServer)
{
    ServerBootstrap(8)
        .option(SockOption::SNDBUF, 1024)
        .option(SockOption::RCVBUF, 1024)
        .childOption(SockOption::KEEP_ALIVE, true)
        .childHandler(
            [](SocketChannel& channel)
            {
                channel.pipeline().addLast(new TestServerChannelHandler());
            })
        .bind(8080)
        .sync();
}

class TestClientChannelHandler : public SocketChannelHandler
{
public:
    void channelConnect(SocketChannelContext& channelContext, const InetSockAddress& localAddress,
                        const InetSockAddress& peerAddress) override
    {
        LOGS_DEBUG << "isActive=" << channelContext.isActive();
        LOGS_DEBUG << "Client channelConnect ====local address:" << localAddress.toString()
                   << " server address:" << peerAddress.toString();
        channelContext.write("Hello Server");
        channelContext.shutdown();
    }

    void channelDisconnect(SocketChannelContext& channelContext) override
    {
        LOGS_DEBUG << "Client channelDisconnect:" << channelContext.peerAddress().toString();
    }
};

TEST(TcpServerBootstrapTest, TcpClient)
{
    Bootstrap()
        .option(SockOption::SNDBUF, 1024)
        .option(SockOption::RCVBUF, 1024)
        .retry(true, 3000)
        //        .channelHandler(new TestClientChannelHandler())
        .channelHandler(
            [](SocketChannel& channel)
            {
                channel.pipeline().addLast(new TestClientChannelHandler);
            })
        .connect("127.0.0.1", 8080)
        .sync();
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}