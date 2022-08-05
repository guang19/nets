//
// Created by guang19 on 2022/5/19.
//

#include "nets/net/core/SocketChannel.h"

#include "nets/base/log/Logging.h"
#include "nets/net/core/ByteBuffer.h"

namespace nets::net
{
    SocketChannel::SocketChannel(FdType sockFd, const InetSockAddress& peerAddress, EventLoopRawPtr eventLoop)
        : Channel(eventLoop), sockFd_(sockFd), localAddress_(), peerAddress_(peerAddress), channelHandlerPipeline_(this)
    {
        socket::getLocalAddress(sockFd_, localAddress_.sockAddr());
        socket::setSockNonBlock(sockFd_);
    }

    SocketChannel::~SocketChannel()
    {
        socket::closeFd(sockFd_);
    }

    void SocketChannel::setChannelOptions(const ChannelOptionList& channelOptions)
    {
        for (const auto& channelOption: channelOptions)
        {
            setChannelOption(channelOption);
        }
    }

    void SocketChannel::handleReadEvent()
    {
        ByteBuffer byteBuffer {};
        byteBuffer.writeBytes(*this);
    }

    void SocketChannel::handleWriteEvent() {}

    void SocketChannel::handleErrorEvent() {}
} // namespace nets::net