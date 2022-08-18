//
// Created by guang19
//

#include "nets/net/client/Bootstrap.h"

using namespace nets::net;

class TestSharedClientChannelHandler : public ChannelHandler
{
public:
    void channelConnect(ChannelContext& channelContext, const InetSockAddress& localAddress,
                        const InetSockAddress& peerAddress) override
    {
        LOGS_DEBUG << "isActive=" << channelContext.isActive();
        LOGS_DEBUG << "Client channelConnect ====local address:" << localAddress.toString()
                   << " server address:" << peerAddress.toString();
//        channelContext.write("Hello Server");
//        channelContext.shutdown();
    }

    void channelDisconnect(ChannelContext& channelContext) override
    {
        LOGS_DEBUG << "Client channelDisconnect";
    }

    void channelRead(ChannelContext& channelContext, ByteBuffer& message) override
    {
        LOGS_DEBUG << "Client recv server message is:" << message.toString();
    }

    void exceptionCaught(ChannelContext& channelContext, const std::exception& exception) override
    {
        LOGS_DEBUG << "Client exceptionCaught";
    }

private:
    int32_t sharedNum = 0;
};

int main(int argc, char** argv)
{
    Bootstrap bootstrap;
    bootstrap.option(NTcpSendBuf, 20480)
        .channelHandler(new TestSharedClientChannelHandler())
        .channelHandler(
            [](SocketChannel& channel) {

            })
        .connect("127.0.0.1", 8080)
        .sync();
}
