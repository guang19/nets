//
// Created by guang19
//

#include "nets/net/client/Bootstrap.h"

using namespace nets::net;

class TestClientChannelHandler : public ChannelHandler
{
public:
    void channelConnect(ChannelContext& channelContext, const InetSockAddress& localAddress,
                        const InetSockAddress& peerAddress) override
    {
        LOGS_DEBUG << "isActive=" << channelContext.isActive();
        LOGS_DEBUG << "Client channelConnect ====local address:" << localAddress.toString()
                   << " server address:" << peerAddress.toString();
        //        ByteBuffer byteBuffer {};
        //        byteBuffer.writeInt8(1);
        //        channelContext.write(byteBuffer);

        channelContext.write("Hello Server");
        channelContext.shutdown();

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
    }

    void channelDisconnect(ChannelContext& channelContext) override
    {
        LOGS_DEBUG << "Client channelDisconnect:" << channelContext.peerAddress().toString();
    }

    void channelRead(ChannelContext& channelContext, ByteBuffer& message) override
    {
        LOGS_DEBUG << "Client recv server message is:" << message.toString();
        //        channelContext.write(message);
    }

    void channelWriteComplete(ChannelContext& channelContext) override
    {
        LOGS_DEBUG << "Client channelWriteComplete";
        LOGS_DEBUG << "isActive=" << channelContext.isActive();
    }

    void exceptionCaught(ChannelContext& channelContext, const ::std::exception& exception) override
    {
        LOGS_DEBUG << "Client exceptionCaught";
    }

private:
    ::int32_t sharedNum = 0;
};

int main(int argc, char** argv)
{
    Bootstrap bootstrap;
    bootstrap.option(NTcpSendBuffer, 1024)
        .option(NTcpRecvBuffer, 1024)
        .retry( true, 3000)
        //        .channelHandler(new TestClientChannelHandler())
        .channelHandler(
            [](SocketChannel& channel)
            {
                channel.pipeline().addLast(new TestClientChannelHandler);
            })
        .connect("127.0.0.1", 8080)
        .sync();
}