//
// Created by guang19 on 2022/5/18.
//

#include "nets/net/server/ServerSocketChannel.h"

#include "nets/base/log/Logging.h"

namespace nets::net
{
    ServerSocketChannel::ServerSocketChannel(EventLoopRawPtr eventLoop)
        : Channel(eventLoop), sockFd_(socket::InvalidFd), idleFd_(socket::createIdleFd()), acceptor_(new Acceptor())
    {
    }

    ServerSocketChannel::~ServerSocketChannel()
    {
        socket::closeFd(sockFd_);
        socket::closeFd(idleFd_);
    }

    void ServerSocketChannel::bind(const InetSockAddress& sockAddress)
    {
        sockFd_ = socket::createTcpSocket(sockAddress.ipFamily());
        socket::setSockAddrReuse(sockFd_, true);
        socket::setSockPortReuse(sockFd_, true);
        socket::setSockNonBlock(sockFd_, true);
        socket::bind(sockFd_, sockAddress.csockAddr());
        socket::listen(sockFd_);
        channelHandlerPipeline().addLast(acceptor_);
        addEvent(EReadEvent);
        if (!registerTo())
        {
            LOGS_FATAL << "Register ServerSocketChannel failed";
        }
    }

    void ServerSocketChannel::handleReadEvent() {}

    void ServerSocketChannel::handleWriteEvent() {}

    void ServerSocketChannel::handleErrorEvent() {}
} // namespace nets::net