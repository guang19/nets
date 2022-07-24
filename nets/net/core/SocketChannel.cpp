//
// Created by guang19 on 2022/5/19.
//

#include "nets/net/core/SocketChannel.h"

namespace nets::net
{
    SocketChannel::SocketChannel(FdType sockFd, const InetSockAddress& peerAddress, EventLoopRawPtr eventLoop)
        : Channel(eventLoop), sockFd_(sockFd), peerAddress_(peerAddress), channelContext_(this)
    {
        socket::setSockNonBlock(sockFd_);
    }

    void SocketChannel::handleReadEvent() {}

    void SocketChannel::handleWriteEvent() {}

    void SocketChannel::handleErrorEvent() {}
} // namespace nets::net