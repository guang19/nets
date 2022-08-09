//
// Created by guang19 on 2022/5/23.
//

#include "nets/net/core/ChannelHandler.h"

#include "nets/base/log/Logging.h"
#include "nets/net/core/ByteBuffer.h"

namespace nets::net
{

    void ChannelHandler::channelConnect(ChannelContext& channelContext, const InetSockAddress& localAddress,
                                        const InetSockAddress& peerAddress)
    {
        LOGS_DEBUG << "ChannelHandler::channelConnect";
    }

    void ChannelHandler::channelDisconnect(ChannelContext& channelContext)
    {
        LOGS_DEBUG << "ChannelHandler::channelDisconnect";
    }

    void ChannelHandler::channelRead(ChannelContext& channelContext, ByteBuffer& message)
    {
        LOGS_DEBUG << "ChannelHandler::channelRead";
    }

    void ChannelHandler::exceptionCaught(ChannelContext& channelContext, const ::std::exception& exception)
    {
        LOGS_DEBUG << "ChannelHandler::exceptionCaught";
    }
} // namespace nets::net