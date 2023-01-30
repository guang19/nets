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

// @brief Test for TcpServer

#include <gtest/gtest.h>

#include "nets/net/bootstrap/Bootstrap.h"
#include "nets/net/bootstrap/ServerBootstrap.h"

using namespace nets;

LoggerPtr testLogger = LOGGER_MGR->getLogger("TcpBigDataSendTest");

class TestServerChannelHandler : public SocketChannelHandler
{
public:
    void channelConnect(SocketChannelContext& channelContext, const InetSockAddress& localAddress,
                        const InetSockAddress& peerAddress) override
    {
        NETS_LOG_DEBUG(testLogger) << "isActive=" << channelContext.isActive();
        NETS_LOG_DEBUG(testLogger) << "Server channelConnect ====local address:" << localAddress.toString()
                                   << " client address:" << peerAddress.toString();
        fireChannelConnect(channelContext, localAddress, peerAddress);
    }

    void channelDisconnect(SocketChannelContext& channelContext) override
    {
        NETS_LOG_DEBUG(testLogger) << "Server channelDisconnect:" << channelContext.peerAddress().toString();
        fireChannelDisconnect(channelContext);
    }

    void channelRead(SocketChannelContext& channelContext, ByteBuffer& message) override
    {
        NETS_LOG_DEBUG(testLogger) << "Server recv client message size=" << message.readableBytes()
                                   << " content=" << message.toString();
        fireChannelRead(channelContext, message);
    }

    void writeComplete(SocketChannelContext& channelContext)
    {
        NETS_LOG_DEBUG(testLogger) << "Server writeComplete";
    }
};

TEST(TcpBigDataSendTest, TcpServer)
{
    ServerBootstrap(8)
        .option(SockOption::SNDBUF, 1024)
        .option(SockOption::RCVBUF, 1024)
        //        .childHandler(::std::shared_ptr<SocketChannelHandler>(nullptr))
        .childHandler(
            [](SocketChannel& channel)
            {
                channel.pipeline().addLast(::std::shared_ptr<SocketChannelHandler>(new TestServerChannelHandler()));
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
        NETS_LOG_DEBUG(testLogger) << "isActive=" << channelContext.isActive();
        NETS_LOG_DEBUG(testLogger) << "Client channelConnect ====local address:" << localAddress.toString()
                                   << " server address:" << peerAddress.toString();
        // 1746
        StringType content1 = "一段中文内容一段中文内容一段中文内容一段中文内容一段中文内容一段中文内容"
                              "一段中文内容一段中文内容一段中文内容一段中文内容一段中文内容一段中文内容一段中文内容"
                              "一段中文内容一段中文内容一段中文内容一段中文内容一段中文内容一段中文内容一段中文内容"
                              "一段中文内容一段中文内容一段中文内容一段中文内容一段中文内容一段中文内容一段中文内容"
                              "一段中文内容一段中文内容一段中文内容一段中文内容一段中文内容一段中文内容一段中文内容"
                              "一段中文内容一段中文内容一段中文内容一段中文内容一段中文内容一段中文内容一段中文内容"
                              "一段中文内容一段中文内容一段中文内容一段中文内容一段中文内容一段中文内容一段中文内容"
                              "一段中文内容一段中文内容一段中文内容一段中文内容一段中文内容一段中文内容一段中文内容"
                              "一段中文内容一段中文内容一段中文内容一段中文内容一段中文内容一段中文内容一段中文内容"
                              "一段中文内容一段中文内容一段中文内容一段中文内容一段中文内容一段中文内容一段中文内容"
                              "一段中文内容一段中文内容一段中文内容一段中文内容一段中文内容一段中文内容一段中文内容"
                              "一段中文内容一段中文内容一段中文内容一段中文内容一段中文内容一段中文内容一段中文内容"
                              "一段中文内容一段中文内容一段中文内容一段中文内容一段中文内容一段中文内容一段中文内容"
                              "一段中文内容一段中文内容一段中文内容一段中文内容一段中文内容一段中文内容一段中文内容";
        channelContext.write(content1,
                             [&, bytes = content1.length()](SocketChannelContext& ctx)
                             {
                                 writeComplete(ctx, bytes);
                             });
        // 6030
        StringType content2 = "一段中文内容一段中文内容一段中文内容一段中文内容一段中文内容一段中文内容一段中文内容一段中"
                              "文内容一段中文内容一段中文内容一段中文内容一段中文内容一段中文内容"
                              "一段中文内容一段中文内容一段中文内容一段中文内容一段中文内容一段中文内容一段中文内容一段中"
                              "文内容一段中文内容一段中文内容一段中文内容一段中文内容一段中文内容一段中文内容"
                              "一段中文内容一段中文内容一段中文内容一段中文内容一段中文内容一段中文内容一段中文内容一段中"
                              "文内容一段中文内容一段中文内容一段中文内容一段中文内容一段中文内容一段中文内容"
                              "一段中文内容一段中文内容一段中文内容一段中文内容一段中文内容一段中文内容一段中文内容一段中"
                              "文内容一段中文内容一段中文内容一段中文内容一段中文内容一段中文内容一段中文内容"
                              "一段中文内容一段中文内容一段中文内容一段中文内容一段中文内容一段中文内容一段中文内容一段中"
                              "文内容一段中文内容一段中文内容一段中文内容一段中文内容一段中文内容一段中文内容"
                              "一段中文内容一段中文内容一段中文内容一段中文内容一段中文内容一段中文内容一段中文内容一段中"
                              "文内容一段中文内容一段中文内容一段中文内容一段中文内容一段中文内容一段中文内容"
                              "一段中文内容一段中文内容一段中文内容一段中文内容一段中文内容一段中文内容一段中文内容一段中"
                              "文内容一段中文内容一段中文内容一段中文内容一段中文内容一段中文内容一段中文内容"
                              "一段中文内容一段中文内容一段中文内容一段中文内容一段中文内容一段中文内容一段中文内容一段中"
                              "文内容一段中文内容一段中文内容一段中文内容一段中文内容一段中文内容一段中文内容"
                              "一段中文内容一段中文内容一段中文内容一段中文内容一段中文内容一段中文内容一段中文内容一段中"
                              "文内容一段中文内容一段中文内容一段中文内容一段中文内容一段中文内容一段中文内容"
                              "一段中文内容一段中文内容一段中文内容一段中文内容一段中文内容一段中文内容一段中文内容一段中"
                              "文内容一段中文内容一段中文内容一段中文内容一段中文内容一段中文内容一段中文内容"
                              "一段中文内容一段中文内容一段中文内容一段中文内容一段中文内容一段中文内容一段中文内容一段中"
                              "文内容一段中文内容一段中文内容一段中文内容一段中文内容一段中文内容一段中文内容"
                              "一段中文内容一段中文内容一段中文内容一段中文内容一段中文内容一段中文内容一段中文内容一段中"
                              "文内容一段中文内容一段中文内容一段中文内容一段中文内容一段中文内容一段中文内容"
                              "一段中文内容一段中文内容一段中文内容一段中文内容一段中文内容一段中文内容一段中文内容一段中"
                              "文内容一段中文内容一段中文内容一段中文内容一段中文内容一段中文内容一段中文内容"
                              "一段中文内容一段中文内容一段中文内容一段中文内容一段中文内容一段中文内容一段中文内容一段中"
                              "文内容一段中文内容一段中文内容一段中文内容一段中文内容一段中文内容一段中文内容"
                              "一段中文内容一段中文内容一段中文内容一段中文内容一段中文内容一段中文内容一段中文内容一段中"
                              "文内容一段中文内容一段中文内容一段中文内容一段中文内容一段中文内容一段中文内容"
                              "一段中文内容一段中文内容一段中文内容一段中文内容一段中文内容一段中文内容一段中文内容一段中"
                              "文内容一段中文内容一段中文内容一段中文内容一段中文内容一段中文内容一段中文内容"
                              "一段中文内容一段中文内容一段中文内容一段中文内容一段中文内容一段中文内容一段中文内容一段中"
                              "文内容一段中文内容一段中文内容一段中文内容一段中文内容一段中文内容一段中文内容"
                              "一段中文内容一段中文内容一段中文内容一段中文内容一段中文内容一段中文内容一段中文内容一段中"
                              "文内容一段中文内容一段中文内容一段中文内容一段中文内容一段中文内容一段中文内容"
                              "一段中文内容一段中文内容一段中文内容一段中文内容一段中文内容一段中文内容一段中文内容一段中"
                              "文内容一段中文内容一段中文内容一段中文内容一段中文内容一段中文内容一段中文内容"
                              "一段中文内容一段中文内容一段中文内容一段中文内容一段中文内容一段中文内容一段中文内容一段中"
                              "文内容一段中文内容一段中文内容一段中文内容一段中文内容一段中文内容一段中文内容"
                              "一段中文内容一段中文内容一段中文内容一段中文内容一段中文内容一段中文内容一段中文内容一段中"
                              "文内容一段中文内容一段中文内容一段中文内容一段中文内容一段中文内容一段中文内容"
                              "一段中文内容一段中文内容一段中文内容一段中文内容一段中文内容一段中文内容一段中文内容一段中"
                              "文内容一段中文内容一段中文内容一段中文内容一段中文内容一段中文内容一段中文内容"
                              "一段中文内容一段中文内容一段中文内容一段中文内容一段中文内容一段中文内容一段中文内容一段中"
                              "文内容一段中文内容一段中文内容一段中文内容一段中文内容一段中文内容一段中文内容"
                              "一段中文内容一段中文内容一段中文内容一段中文内容一段中文内容一段中文内容一段中文内容一段中"
                              "文内容一段中文内容一段中文内容一段中文内容一段中文内容一段中文内容一段中文内容";
        channelContext.write(content2,
                             [&, bytes = content2.length()](SocketChannelContext& ctx)
                             {
                                 writeComplete(ctx, bytes);
                             });
        fireChannelConnect(channelContext, localAddress, peerAddress);
    }

    void channelDisconnect(SocketChannelContext& channelContext) override
    {
        NETS_LOG_DEBUG(testLogger) << "Client channelDisconnect:" << channelContext.peerAddress().toString();
        fireChannelDisconnect(channelContext);
    }

    void channelRead(SocketChannelContext& channelContext, ByteBuffer& message) override
    {
        NETS_LOG_DEBUG(testLogger) << "Client recv server message size=" << message.readableBytes()
                                   << " content=" << message.toString();
        fireChannelRead(channelContext, message);
    }

    void writeComplete(SocketChannelContext& channelContext, SizeType bytes)
    {
        NETS_LOG_DEBUG(testLogger) << "Client writeComplete " << bytes;
    }
};

TEST(TcpBigDataSendTest, TcpClient)
{
    Bootstrap()
        .option(SockOption::SNDBUF, 1024)
        .option(SockOption::RCVBUF, 1024)
        .retry(true, 3000)
        //                .channelHandler(::std::shared_ptr<SocketChannelHandler>(nullptr))
        .channelHandler(
            [](SocketChannel& channel)
            {
                channel.pipeline().addLast(::std::shared_ptr<SocketChannelHandler>(new TestClientChannelHandler()));
            })
        .connect("127.0.0.1", 8080)
        .sync();
}