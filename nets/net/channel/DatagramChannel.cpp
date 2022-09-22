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
        : Channel(eventLoop), sockFd_(socket::InvalidFd), writeBuffer_(),
          channelHandlerPipeline_(new DatagramChannelContext(this)), channelOptions_()
    {
        channelOptions_.push_back(SO_ReuseAddr);
        channelOptions_.push_back(SO_ReusePort);
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
        bool needBind = true;
        if (localAddress.isInValid())
        {
            sockFd_ = socket::createUdpSocket();
            needBind = false;
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
        if (needBind)
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

    void DatagramChannel::write(const void* message, SizeType length, const InetSockAddress& recipient)
    {
        doWrite(message, length, recipient);
    }

    void DatagramChannel::write(const StringType& message, const InetSockAddress& recipient) {}

    void DatagramChannel::write(const ByteBuffer& message, const InetSockAddress& recipient) {}

    void DatagramChannel::write(const DatagramPacket& message) {}

    void DatagramChannel::handleReadEvent() {}

    void DatagramChannel::handleWriteEvent() {}

    void DatagramChannel::handleErrorEvent() {}

    void DatagramChannel::doWrite(const void* data, SizeType length, const InetSockAddress& recipient)
    {
        if (length == 0 || recipient.isInValid())
        {
            return;
        }
        // the writeBuffer has residual data waiting to be sent, append the data to the end of the writeBuffer
        if (!writeBuffer_.empty())
        {
        }
        else
        {
            // write directly
            doWriteDirectly(data, length, recipient);
        }
    }

    void DatagramChannel::doWriteDirectly(const void* data, SizeType length, const InetSockAddress& recipient) {}
} // namespace nets::net