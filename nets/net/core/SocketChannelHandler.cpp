//
// Created by guang19
//

#include "nets/net/core/SocketChannelHandler.h"

#include "nets/base/log/Logging.h"
#include "nets/net/core/ByteBuffer.h"

namespace nets::net
{
    void SocketChannelHandler::channelConnect(ChannelContext& channelContext, const InetSockAddress& localAddress,
                                              const InetSockAddress& peerAddress)
    {
        LOGS_DEBUG << "SocketChannelHandler::channelConnect";
    }

    void SocketChannelHandler::channelDisconnect(ChannelContext& channelContext)
    {
        LOGS_DEBUG << "SocketChannelHandler::channelDisconnect";
    }

    void SocketChannelHandler::channelRead(ChannelContext& channelContext, ByteBuffer& message)
    {
        LOGS_DEBUG << "SocketChannelHandler::channelRead";
    }
} // namespace nets::net
