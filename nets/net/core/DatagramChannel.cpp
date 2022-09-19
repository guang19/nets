//
// Created by guang19
//

#include "nets/net/core/DatagramChannel.h"

#include "nets/base/CommonMacro.h"
#include "nets/base/log/Logging.h"
#include "nets/net/exception/ChannelRegisterException.h"

namespace nets::net
{
    DatagramChannel::DatagramChannel(EventLoopRawPtr eventLoop) : Channel(eventLoop), sockFd_(socket::InvalidFd) {}

    DatagramChannel::~DatagramChannel()
    {
        socket::closeFd(sockFd_);
    }

    FdType DatagramChannel::fd() const
    {
        return sockFd_;
    }

    void DatagramChannel::setChannelOptions(const ChannelOptionList& channelOptions)
    {
        for (const auto& channelOption: channelOptions)
        {
            setChannelOption(channelOption);
        }
    }

    void DatagramChannel::bind(const InetSockAddress& localAddress)
    {
        sockFd_ = socket::createUdpSocket(localAddress.ipFamily());
        socket::setSockNonBlock(sockFd_, true);
        socket::bind(sockFd_, localAddress.sockAddr());
        addEvent(EReadEvent);
        try
        {
            if (!registerTo())
            {
                THROW_FMT(ChannelRegisterException, "DatagramChannel register failed");
            }
        }
        catch (const ChannelRegisterException& exception)
        {
            THROW_FMT(ChannelRegisterException, "DatagramChannel register failed");
            if (isRegistered())
            {
                deregister();
            }
            LOGS_ERROR << "DatagramChannel channelActive failed,cause " << exception.what();
        }
    }

    void DatagramChannel::handleReadEvent() {}

    void DatagramChannel::handleWriteEvent() {}

    void DatagramChannel::handleErrorEvent() {}
} // namespace nets::net