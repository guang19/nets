//
// Created by guang19
//

#include "nets/net/core/DatagramChannel.h"

namespace nets::net
{
    DatagramChannel::DatagramChannel(FdType sockFd, EventLoopRawPtr eventLoop) : Channel(eventLoop), sockFd_(sockFd) {}

    DatagramChannel::~DatagramChannel()
    {
        socket::closeFd(sockFd_);
    }

    FdType DatagramChannel::fd() const
    {
        return sockFd_;
    }

    void DatagramChannel::handleReadEvent()
    {
        Channel::handleReadEvent();
    }

    void DatagramChannel::handleWriteEvent()
    {
        Channel::handleWriteEvent();
    }

    void DatagramChannel::handleErrorEvent()
    {
        Channel::handleErrorEvent();
    }
} // namespace nets::net