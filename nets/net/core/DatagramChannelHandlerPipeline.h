//
// Created by guang19
//

#ifndef NETS_NET_DATAGRAM_CHANNEL_HANDLER_PIPELINE_H
#define NETS_NET_DATAGRAM_CHANNEL_HANDLER_PIPELINE_H

#include <list>

#include "nets/net/core/DatagramChannelContext.h"
#include "nets/net/core/DatagramChannelHandler.h"

namespace nets::net
{
    class DatagramChannelHandlerPipeline : public nets::base::Copyable
    {
    public:
        using DatagramChannelContextRawPtr = DatagramChannelContext*;
        using DatagramChannelContextPtr = ::std::unique_ptr<DatagramChannelContext>;
        using DatagramChannelHandlerRawPtr = DatagramChannelHandler*;
        using DatagramChannelHandlerPtr = ::std::shared_ptr<DatagramChannelHandler>;
        using DatagramChannelHandlerList = ::std::list<DatagramChannelHandlerPtr>;

    public:
        explicit DatagramChannelHandlerPipeline(DatagramChannelContextRawPtr channelContext);
        ~DatagramChannelHandlerPipeline() = default;

        DatagramChannelHandlerPipeline(const DatagramChannelHandlerPipeline& other);
        DatagramChannelHandlerPipeline(DatagramChannelHandlerPipeline&& other) noexcept;
        DatagramChannelHandlerPipeline& operator=(const DatagramChannelHandlerPipeline& other);
        DatagramChannelHandlerPipeline& operator=(DatagramChannelHandlerPipeline&& other) noexcept;

    public:
        inline DatagramChannelContext& context()
        {
            return *channelContext_;
        }

        void addFirst(DatagramChannelHandlerRawPtr channelHandler);
        void addFirst(const DatagramChannelHandlerPtr& channelHandler);
        void addLast(DatagramChannelHandlerRawPtr channelHandler);
        void addLast(const DatagramChannelHandlerPtr& channelHandler);

    public:
        void fireDatagramChannelActive();
        void fireDatagramChannelRead(DatagramPacket& message);

    private:
        DatagramChannelContextPtr channelContext_ {};
        DatagramChannelHandlerList channelHandlers_ {};
    };
} // namespace nets::net

#endif // NETS_NET_DATAGRAM_CHANNEL_HANDLER_PIPELINE_H