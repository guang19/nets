//
// Created by guang19 on 2022/5/18.
//

#include "nets/net/server/ServerSocketChannel.h"

#include <cassert>
#include "nets/net/core/EventLoop.h"
#include "nets/base/log/Logging.h"

namespace nets::net
{
    ServerSocketChannel::ServerSocketChannel(EventLoopRawPtr eventLoop)
        : Channel(eventLoop), sockFd_(socket::InvalidFd), idleFd_(socket::createIdleFd()), channelInitializationCallbacks_()
    {
        if (idleFd_ < 0)
        {
            LOGS_FATAL << "ServerSocketChannel createIdleFd failed";
        }
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
        addEvent(EReadEvent);
        if (!registerTo())
        {
            LOGS_FATAL << "ServerSocketChannel register failed";
        }
    }

    void ServerSocketChannel::handleReadEvent()
    {
        assert(eventLoop_->isInCurrentEventLoop());
        InetSockAddress peerAddr {};
        FdType connFd = socket::InvalidFd;
        if ((connFd = socket::accept(sockFd_, peerAddr.sockAddr(), &idleFd_)) > 0)
        {
            LOGS_DEBUG << "ServerSocketChannel accpet client addr: " << peerAddr.toString();

        }
    }

    void ServerSocketChannel::handleErrorEvent()
    {

    }
} // namespace nets::net