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
        NETS_SYSTEM_LOG_DEBUG << "isActive=" << channelContext.isActive();
        NETS_SYSTEM_LOG_DEBUG << "Server channelConnect ====local address:" << localAddress.toString()
                   << " client address:" << peerAddress.toString();
    }

    void channelDisconnect(SocketChannelContext& channelContext) override
    {
        NETS_SYSTEM_LOG_DEBUG << "Server channelDisconnect:" << channelContext.peerAddress().toString();
    }

    void channelRead(SocketChannelContext& channelContext, ByteBuffer& message) override
    {
        NETS_SYSTEM_LOG_DEBUG << "Server recv client message is:" << message.toString();
        //        channelContext.write(message);
        channelContext.write(message,
                             [this](SocketChannelContext& ctx)
                             {
                                 writeComplete(ctx);
                             });
    }

    void writeComplete(SocketChannelContext& channelContext)
    {
        NETS_SYSTEM_LOG_DEBUG << "Server writeComplete";
        channelContext.write("server writeComplete");
    }
};

TEST(TcpServerBootstrapTest, TcpServer)
{
    ServerBootstrap(8)
        .option(SockOption::SNDBUF, 1024)
        .option(SockOption::RCVBUF, 1024)
        .option(SockOption::BACKLOG, 1028)
//                        .childHandler(new TestServerChannelHandler())
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
        NETS_SYSTEM_LOG_DEBUG << "isActive=" << channelContext.isActive();
        NETS_SYSTEM_LOG_DEBUG << "Client channelConnect ====local address:" << localAddress.toString()
                   << " server address:" << peerAddress.toString();
        //        ByteBuffer byteBuffer {};
        //        byteBuffer.writeInt8(1);
        //        channelContext.write(byteBuffer);

        channelContext.write("Hello Server",
                             [this](SocketChannelContext& ctx)
                             {
                                 writeComplete(ctx);
                             });
        //        channelContext.shutdown();

        //        channelContext.write(
        //            "你好，服务端，这是客户端发来的消息你好，服务端，这是客户端发来的消息你好，服务端，这是客户端发来的消息你"
        //            "好，服务端，这是客户端发来的消息你好，服务端，这是客户端发来的消息你好，服务端，这是客户端发来的消息你好"
        //            "，服务端，这是客户端发来的消息你好，服务端，这是客户端发来的消息你好，服务端，这是客户端发来的消息你好，"
        //            "服务端，这是客户端发来的消息"
        //            "你好，服务端，这是客户端发来的消息你好，服务端，这是客户端发来的消息你好，服务端，这是客户端发来的消息你"
        //            "好，服务端，这是客户端发来的消息这是客户端发来的消息这是客户端发来的消息这是客户端发来的消息这是客户端发来的消"
        //            "息这是客户端发来的消息这是客户端发来的消息"
        //            "这是客户端发来的消息这是客户端发来的消息这是客户端发来的消息这是客户端发来的消息这是客户端发来的消息这是客户端"
        //            "发来的消息这是客户端发来的消息"
        //            "这是客户端发来的消息这是客户端发来的消息这是客户端发来的消息这是客户端发来的消息这是客户端发来的消息这是客户端"
        //            "发来的消息这是客户端发来的消息"
        //            "这是客户端发来的消息这是客户端发来的消息这是客户端发来的消息这是客户端发来的消息这是客户端发来的消息这是客户端"
        //            "发来的消息这是客户端发来的消息"
        //            "这是客户端发来的消息这是客户端发来的消息这是客户端发来的消息这是客户端发来的消息这是客户端发来的消息这是客户端"
        //            "发来的消息这是客户端发来的消息"
        //            "这是客户端发来的消息这是客户端发来的消息这是客户端发来的消息这是客户端发来的消息这是客户端发来的消息这是客户端"
        //            "发来的消息这是客户端发来的消息"
        //            "这是客户端发来的消息这是客户端发来的消息这是客户端发来的消息这是客户端发来的消息这是客户端发来的消息这是客户端"
        //            "发来的消息这是客户端发来的消息"
        //            "这是客户端发来的消息这是客户端发来的消息这是客户端发来的消息这是客户端发来的消息这是客户端发来的消息这是客户端"
        //            "发来的消息这是客户端发来的消息"
        //            "这是客户端发来的消息这是客户端发来的消息这是客户端发来的消息这是客户端发来的消息这是客户端发来的消息这是客户端"
        //            "发来的消息这是客户端发来的消息"
        //            "这是客户端发来的消息这是客户端发来的消息这是客户端发来的消息这是客户端发来的消息这是客户端发来的消息这是客户端"
        //            "发来的消息这是客户端发来的消息"
        //            "这是客户端发来的消息这是客户端发来的消息这是客户端发来的消息这是客户端发来的消息这是客户端发来的消息这是客户端"
        //            "发来的消息这是客户端发来的消息"
        //            "这是客户端发来的消息这是客户端发来的消息这是客户端发来的消息这是客户端发来的消息这是客户端发来的消息这是客户端"
        //            "发来的消息这是客户端发来的消息"
        //            "这是客户端发来的消息这是客户端发来的消息这是客户端发来的消息这是客户端发来的消息这是客户端发来的消息这是客户端"
        //            "发来的消息这是客户端发来的消息"
        //            "结尾");
        //        ByteBuffer buffer;
        //        buffer.writeInt8(1);
        //        channelContext.write(buffer);
    }

    void channelDisconnect(SocketChannelContext& channelContext) override
    {
        NETS_SYSTEM_LOG_DEBUG << "Client channelDisconnect:" << channelContext.peerAddress().toString();
    }

    void channelRead(SocketChannelContext& channelContext, ByteBuffer& message) override
    {
        NETS_SYSTEM_LOG_DEBUG << "Client recv server message is:" << message.toString();
        //        channelContext.write(message);
    }

    void writeComplete(SocketChannelContext& channelContext)
    {
        NETS_SYSTEM_LOG_DEBUG << "Client writeComplete";
        NETS_SYSTEM_LOG_DEBUG << "isActive=" << channelContext.isActive();
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