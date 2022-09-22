//
// Created by guang19
//

#include "nets/net/bootstrap/Bootstrap.h"

using namespace nets::net;

class TestClientChannelHandler : public SocketChannelHandler
{
public:
    void channelConnect(SocketChannelContext& channelContext, const InetSockAddress& localAddress,
                        const InetSockAddress& peerAddress) override
    {
        LOGS_DEBUG << "isActive=" << channelContext.isActive();
        LOGS_DEBUG << "Client channelConnect ====local address:" << localAddress.toString()
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
        LOGS_DEBUG << "Client channelDisconnect:" << channelContext.peerAddress().toString();
    }

    void channelRead(SocketChannelContext& channelContext, ByteBuffer& message) override
    {
        LOGS_DEBUG << "Client recv server message is:" << message.toString();
        //        channelContext.write(message);
    }

    void writeComplete(SocketChannelContext& channelContext)
    {
        LOGS_DEBUG << "Client writeComplete";
        LOGS_DEBUG << "isActive=" << channelContext.isActive();
    }
};

int main(int argc, char** argv)
{
    Bootstrap()
        .option(SO_TcpSendBuffer, 1024)
        .option(SO_TcpRecvBuffer, 1024)
        .retry(true, 3000)
        //        .channelHandler(new TestClientChannelHandler())
        .channelHandler(
            [](SocketChannel& channel)
            {
                channel.pipeline().addLast(new TestClientChannelHandler);
            })
        .connect("127.0.0.1", 8080)
        .sync();
    return 0;
}