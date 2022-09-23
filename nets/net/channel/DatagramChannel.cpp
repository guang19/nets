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
            socket::bind(sockFd_, localAddress);
        }
        addEvent(EReadEvent);
        if (!registerTo())
        {
            THROW_FMT(ChannelRegisterException, "DatagramChannel register failed");
        }
        channelHandlerPipeline_.fireDatagramChannelActive();
    }

    SSizeType DatagramChannel::write(const void* message, SizeType length, const InetSockAddress& recipient)
    {
        return doWrite(message, length, recipient);
    }

    SSizeType DatagramChannel::write(const StringType& message, const InetSockAddress& recipient)
    {
        return doWrite(message.data(), message.length(), recipient);
    }

    SSizeType DatagramChannel::write(const ByteBuffer& message, const InetSockAddress& recipient)
    {
        return doWrite(message.data(), message.readableBytes(), recipient);
    }

    SSizeType DatagramChannel::write(const DatagramPacket& message)
    {
        return doWrite(message.data(), message.length(), message.recipient());
    }

    void DatagramChannel::handleReadEvent() {}

    void DatagramChannel::handleErrorEvent()
    {
        LOGS_ERROR << "SocketChannel handleErrorEvent,errNum=" << socket::getSockError(sockFd_);
    }

    SSizeType DatagramChannel::doWrite(const void* data, SizeType length, const InetSockAddress& recipient)
    {
        if (length == 0 || recipient.isInValid())
        {
            return 0;
        }
        SSizeType bytes = socket::sendTo(sockFd_, data, length, recipient);
        if (bytes < 0)
        {
            LOGS_ERROR << "DatagramChannel sendTo unexpected error,errno=" << errno;
        }
        return bytes;
    }
} // namespace nets::net