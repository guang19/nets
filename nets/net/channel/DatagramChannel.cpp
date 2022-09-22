//
// Created by guang19
//

#include "nets/net/channel/DatagramChannel.h"

#include <cassert>

#include "nets/base/CommonMacro.h"
#include "nets/base/log/Logging.h"
#include "nets/net/exception/ChannelRegisterException.h"

namespace nets::net
{
    DatagramChannel::DatagramChannel(EventLoopRawPtr eventLoop)
        : Channel(eventLoop), sockFd_(socket::InvalidFd), channelHandlerPipeline_(new DatagramChannelContext(this)),
          channelOptions_()
    {
        channelOptions_.push_back(NReuseAddr);
        channelOptions_.push_back(NReusePort);
    }

    DatagramChannel::~DatagramChannel()
    {
        socket::closeFd(sockFd_);
    }

    FdType DatagramChannel::fd() const
    {
        return sockFd_;
    }

    void DatagramChannel::bind(const InetSockAddress& localAddress)
    {
        bool bind = true;
        if (localAddress.isInValid())
        {
            sockFd_ = socket::createUdpSocket();
            bind = false;
        }
        else
        {
            sockFd_ = socket::createUdpSocket(localAddress.ipFamily());
        }
        socket::setSockNonBlock(sockFd_, true);
        for (const auto& channelOption: channelOptions_)
        {
            setChannelOption(channelOption);
        }
        channelOptions_.clear();
        assert(channelOptions_.empty());
        if (bind)
        {
            socket::bind(sockFd_, localAddress.sockAddr());
        }
        addEvent(EReadEvent);
        if (!registerTo())
        {
            THROW_FMT(ChannelRegisterException, "DatagramChannel register failed");
        }
        channelHandlerPipeline_.fireDatagramChannelActive();
    }

    void DatagramChannel::handleReadEvent() {}

    void DatagramChannel::handleWriteEvent() {}

    void DatagramChannel::handleErrorEvent() {}
} // namespace nets::net