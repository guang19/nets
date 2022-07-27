//
// Created by guang19 on 2022/5/23.
//

#include "nets/net/core/ChannelHandler.h"

#include "nets/base/log/Logging.h"

namespace nets::net
{

    void ChannelHandler::channelConnect(ChannelContext& channelContext, const InetSockAddress& peerAddress,
                                        const InetSockAddress& localAddress)
    {
        LOGS_DEBUG << "ChannelHandler::channelConnect";
    }

    void ChannelHandler::channelClose(ChannelContext& channelContext)
    {
        LOGS_DEBUG << "ChannelHandler::channelClose";
    }

    void ChannelHandler::channelDisconnect(ChannelContext& channelContext)
    {
        LOGS_DEBUG << "ChannelHandler::channelDisconnect";
    }

    void ChannelHandler::channelRead(ChannelContext& channelContext, NetPackage& message)
    {
        LOGS_DEBUG << "ChannelHandler::channelRead";
    }

    void ChannelHandler::channelWrite(ChannelContext& channelContext, NetPackage& message)
    {
        LOGS_DEBUG << "ChannelHandler::channelWrite";
    }
} // namespace nets::net