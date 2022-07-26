//
// Created by guang19 on 2022/5/18.
//

#include "nets/net/server/ServerSocketChannel.h"

#include <cassert>

#include "nets/base/log/Logging.h"
#include "nets/net/core/EventLoop.h"

namespace nets::net
{
    ServerSocketChannel::ServerSocketChannel(EventLoopRawPtr eventLoop)
        : Channel(eventLoop), sockFd_(socket::InvalidFd), idleFd_(socket::createIdleFd()), nextEventLoopFn_(),
          childOptions_(), childHandlers_(), childInitializationCallback_()
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
        socket::setSockReuseAddr(sockFd_, true);
        socket::setSockReusePort(sockFd_, true);
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
            auto clientSocketChannel = ::std::make_shared<SocketChannel>(connFd, peerAddr, nextEventLoopFn_());
            for (auto& childHandler: childHandlers_)
            {
                clientSocketChannel->channelHandlerPipeline()->addLast(childHandler);
            }
            if (childInitializationCallback_ != nullptr)
            {
                childInitializationCallback_(clientSocketChannel);
            }
        }
    }

    void ServerSocketChannel::handleErrorEvent()
    {
        LOGS_ERROR << "ServerSocketChannel occurred system error";
        deregister();
    }
} // namespace nets::net