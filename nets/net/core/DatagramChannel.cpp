//
// Created by guang19
//

#include "nets/net/core/DatagramChannel.h"

#include "nets/base/CommonMacro.h"
#include "nets/net/exception/ChannelRegisterException.h"

namespace nets::net
{
    DatagramChannel::DatagramChannel(EventLoopRawPtr eventLoop)
        : Channel(eventLoop), sockFd_(socket::InvalidFd), channelOptions_()
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
        sockFd_ = socket::createUdpSocket(localAddress.ipFamily());
        socket::setSockNonBlock(sockFd_, true);
        for (const auto& channelOption: channelOptions_)
        {
            setChannelOption(channelOption);
        }
        socket::bind(sockFd_, localAddress.sockAddr());
        addEvent(EReadEvent);
        if (!registerTo())
        {
            THROW_FMT(ChannelRegisterException, "ServerSocketChannel register failed");
        }
    }

    void DatagramChannel::handleReadEvent() {}

    void DatagramChannel::handleWriteEvent() {}

    void DatagramChannel::handleErrorEvent() {}
} // namespace nets::net