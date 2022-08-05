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
        : Channel(eventLoop), sockFd_(socket::InvalidFd), idleFd_(socket::createIdleFd()), backlog_(0), channelOptions_(),
          nextEventLoopFn_(), childOptions_(), childHandlers_(), childInitializationCallback_()
    {
        if (idleFd_ < 0)
        {
            THROW_FMT(::std::runtime_error, "ServerSocketChannel createIdleFd failed");
        }
        channelOptions_.push_back(NReuseAddr);
        channelOptions_.push_back(NReusePort);
        channelOptions_.push_back(NBackLog);
        childOptions_.push_back(NReuseAddr);
        childOptions_.push_back(NReusePort);
    }

    ServerSocketChannel::~ServerSocketChannel()
    {
        socket::closeFd(sockFd_);
        socket::closeFd(idleFd_);
    }

    void ServerSocketChannel::setChannelOptions(const ChannelOptionList& channelOptions)
    {
        channelOptions_.insert(channelOptions_.end(), channelOptions.begin(), channelOptions.end());
    }

    void ServerSocketChannel::setChildOptions(const ChannelOptionList& childOptions)
    {
        childOptions_.insert(childOptions_.end(), childOptions.begin(), childOptions.end());
    }

    void ServerSocketChannel::setChannelOptions()
    {
        for (const auto& channelOption: channelOptions_)
        {
            setChannelOption(channelOption);
        }
    }

    void ServerSocketChannel::bind(const InetSockAddress& sockAddress)
    {
        sockFd_ = socket::createTcpSocket(sockAddress.ipFamily());
        setChannelOptions();
        socket::setSockNonBlock(sockFd_, true);
        socket::bind(sockFd_, sockAddress.sockAddr());
        socket::listen(sockFd_, backlog_);
        addEvent(EReadEvent);
        if (!registerTo())
        {
            THROW_FMT(::std::runtime_error, "ServerSocketChannel register failed");
        }
    }

    void ServerSocketChannel::handleReadEvent()
    {
        assert(eventLoop_->isInCurrentEventLoop());
        InetSockAddress peerAddr {};
        FdType connFd = socket::InvalidFd;
        if ((connFd = socket::accept(sockFd_, peerAddr.sockAddr(), &idleFd_)) > 0)
        {
            LOGS_DEBUG << "ServerSocketChannel accpet client addr:" << peerAddr.toString();
            auto socketChannel = ::std::make_shared<SocketChannel>(connFd, peerAddr, nextEventLoopFn_());
            socketChannel->setChannelOptions(childOptions_);
            for (const auto& childHandler: childHandlers_)
            {
                assert(childHandler.use_count() == 1);
                socketChannel->pipeline().addLast(childHandler);
            }
            if (childInitializationCallback_ != nullptr)
            {
                childInitializationCallback_(*socketChannel);
            }
            socketChannel->addEvent(EReadEvent);
            if (!socketChannel->registerTo())
            {
                LOGS_ERROR << "SocketChannel register failed";
            }
            InetSockAddress localAddr {};
            socket::getLocalAddress(connFd, localAddr.sockAddr());
            try
            {
                socketChannel->pipeline().fireChannelConnect(localAddr, peerAddr);
            }
            catch (const ::std::exception& e)
            {
                socketChannel->pipeline().fireExceptionCaught(e);
            }
        }
    }

    void ServerSocketChannel::handleErrorEvent()
    {
        deregister();
        LOGS_FATAL << "ServerSocketChannel occurred system error";
    }
} // namespace nets::net