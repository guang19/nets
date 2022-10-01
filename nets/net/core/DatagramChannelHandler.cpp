//
// Created by guang19
//

#include "nets/net/core/DatagramChannelHandler.h"

#include "nets/base/log/Logging.h"
#include "nets/net/core/DatagramChannelContext.h"

namespace nets
{
    void DatagramChannelHandler::channelActive(DatagramChannelContext& channelContext)
    {
        LOGS_DEBUG << "DatagramChannelHandler::channelActive";
    }

    void DatagramChannelHandler::channelRead(DatagramChannelContext& channelContext, DatagramPacket& message)
    {
        LOGS_DEBUG << "DatagramChannelHandler::channelRead";
    }
} // namespace nets