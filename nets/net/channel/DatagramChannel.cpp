// MIT License
//
// Copyright (c) 2022 guang19
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

// @brief The encapsulation of udp server or udp client operations

#include "nets/net/channel/DatagramChannel.h"

#include <cassert>

#include "nets/base/Common.h"
#include "nets/base/log/Logger.h"
#include "nets/net/exception/ChannelRegisterException.h"

namespace nets
{
    namespace
    {
        static const SizeType kRecvPacketSize = kDefaultUdpSockRecvBufferSize >> 2;
    } // namespace

    DatagramChannel::DatagramChannel(EventLoopRawPtr eventLoop)
        : Channel(eventLoop), sockFd_(socket::kInvalidFd), channelHandlerPipeline_(new DatagramChannelContext(this)),
          channelOptions_()
    {
        channelOptions_[SockOption::REUSE_ADDR] = true;
        channelOptions_[SockOption::REUSE_PORT] = true;
    }

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
        for (const auto& it : channelOptions)
        {
            channelOptions_.insert_or_assign(it.first, it.second);
        }
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
        for (const auto& channelOption : channelOptions_)
        {
            setChannelOption(channelOption.first, channelOption.second);
        }
        channelOptions_.clear();
        assert(channelOptions_.empty());
        if (needBind)
        {
            socket::bind(sockFd_, localAddress);
        }
        addEvent(kReadEvent);
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

    bool DatagramChannel::joinIpv4MulticastGroupByIfAddr(const StringType& multicastAddr, const StringType& ifAddr)
    {
        return socket::addIpMemberShipByIfAddr(sockFd_, multicastAddr, ifAddr);
    }

    bool DatagramChannel::joinIpv4MulticastGroupByIfIndex(const StringType& multicastAddr, const StringType& inf)
    {
        return socket::addIpMemberShipByIfIndex(sockFd_, multicastAddr, inf);
    }

    bool DatagramChannel::joinIpv6MulticastGroup(const StringType& multicastAddr, const StringType& inf)
    {
        return socket::addIpv6MemberShip(sockFd_, multicastAddr, inf);
    }

    bool DatagramChannel::leaveIpv4MulticastGroupByIfAddr(const StringType& multicastAddr, const StringType& ifAddr)
    {
        return socket::dropIpMemberShipByIfAddr(sockFd_, multicastAddr, ifAddr);
    }

    bool DatagramChannel::leaveIpv4MulticastGroupByIfIndex(const StringType& multicastAddr, const StringType& inf)
    {
        return socket::dropIpMemberShipByIfIndex(sockFd_, multicastAddr, inf);
    }

    bool DatagramChannel::leaveIpv6MulticastGroup(const StringType& multicastAddr, const StringType& inf)
    {
        return socket::dropIpv6MemberShip(sockFd_, multicastAddr, inf);
    }

    void DatagramChannel::handleReadEvent()
    {
        ByteBuffer byteBuffer(kRecvPacketSize);
        InetSockAddress srcAddr {};
        SSizeType bytes = byteBuffer.writeBytes(*this, byteBuffer.writableBytes(), srcAddr);
        if (bytes > 0)
        {
            DatagramPacket datagramPacket(::std::move(byteBuffer), ::std::move(srcAddr));
            channelHandlerPipeline_.fireDatagramChannelRead(datagramPacket);
        }
        else if (bytes < 0)
        {
            handleReadError(errno);
        }
        else
        {
            NETS_SYSTEM_LOG_WARN << "DatagramChannel handleReadEvent read 0 bytes";
        }
    }

    void DatagramChannel::handleErrorEvent()
    {
        NETS_SYSTEM_LOG_ERROR << "DatagramChannel handleErrorEvent,errNum=" << socket::getSockError(sockFd_);
    }

    SSizeType DatagramChannel::doWrite(const void* data, SizeType length, const InetSockAddress& recipient)
    {
        if (length == 0 || recipient.isInValid())
        {
            return 0;
        }
        SSizeType bytes = socket::sendTo(sockFd_, data, length, recipient);
        if (bytes <= 0)
        {
            NETS_SYSTEM_LOG_ERROR << "DatagramChannel occurred unexpected exception while sendto,errno=" << errno;
        }
        return bytes;
    }

    void DatagramChannel::handleReadError(Int32Type errNum)
    {
        switch (errNum)
        {
            // EAGAIN/EWOULDBLOCK is not an error
            // EWOULDBLOCK
            case EAGAIN:
            case EINTR:
                break;
            // error
            case EBADF:
            case ECONNRESET:
            case EINVAL:
            case EIO:
            case EPERM:
            case EISDIR:
            case ENOBUFS:
            case ENOMEM:
            case ENOTCONN:
            case ENOTSOCK:
            case EOPNOTSUPP:
            case ETIMEDOUT:
            default:
                NETS_SYSTEM_LOG_ERROR << "DatagramChannel handleReadError unexpected error,errno=" << errNum;
                break;
        }
    }
} // namespace nets