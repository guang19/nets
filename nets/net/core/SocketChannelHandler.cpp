//
// Created by guang19
//

#include "nets/net/core/SocketChannelHandler.h"

#include "nets/base/log/Logging.h"

namespace nets::net
{
    void SocketChannelHandler::channelConnect(SocketChannelContext& channelContext, const InetSockAddress& localAddress,
                                              const InetSockAddress& peerAddress)
    {
        LOGS_DEBUG << "SocketChannelHandler::channelConnect";
    }

    void SocketChannelHandler::channelDisconnect(SocketChannelContext& channelContext)
    {
        LOGS_DEBUG << "SocketChannelHandler::channelDisconnect";
    }

    void SocketChannelHandler::channelRead(SocketChannelContext& channelContext, ByteBuffer& message)
    {
        LOGS_DEBUG << "SocketChannelHandler::channelRead";
    }
} // namespace nets::net
