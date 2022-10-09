//
// Created by guang19
//

#include "nets/net/core/DatagramChannelHandler.h"

#include "nets/base/log/Logger.h"
#include "nets/net/core/DatagramChannelContext.h"

namespace nets
{
    void DatagramChannelHandler::channelActive(DatagramChannelContext& channelContext)
    {
        NETS_SYSTEM_LOG_DEBUG << "DatagramChannelHandler::channelActive";
    }

    void DatagramChannelHandler::channelRead(DatagramChannelContext& channelContext, DatagramPacket& message)
    {
        NETS_SYSTEM_LOG_DEBUG << "DatagramChannelHandler::channelRead";
    }
} // namespace nets