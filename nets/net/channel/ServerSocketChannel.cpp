//
// Created by guang19
//

#include "nets/net/channel/ServerSocketChannel.h"

#include <cassert>

#include "nets/base/log/Logger.h"
#include "nets/net/core/EventLoop.h"
#include "nets/net/exception/ChannelRegisterException.h"
#include "nets/net/exception/ServerSocketChannelException.h"

namespace nets
{
    ServerSocketChannel::ServerSocketChannel(EventLoopRawPtr eventLoop)
        : Channel(eventLoop), sockFd_(socket::kInvalidFd), idleFd_(socket::createIdleFd()), backlog_(0), channelOptions_(),
          nextEventLoopFn_(), childOptions_(), childHandlers_(), childInitializationCallback_()
    {
        if (idleFd_ < 0)
        {
            THROW_FMT(::std::runtime_error, "ServerSocketChannel createIdleFd failed");
        }
        // default options
        channelOptions_[SockOption::REUSE_ADDR] = true;
        channelOptions_[SockOption::REUSE_PORT] = true;
        channelOptions_[SockOption::BACKLOG] = kDefaultMaximumOfBackLog;
        childOptions_[SockOption::REUSE_ADDR] = true;
        childOptions_[SockOption::REUSE_PORT] = true;
    }

    ServerSocketChannel::~ServerSocketChannel()
    {
        socket::closeFd(sockFd_);
        socket::closeFd(idleFd_);
    }

    FdType ServerSocketChannel::fd() const
    {
        return sockFd_;
    }

    void ServerSocketChannel:: bind(const InetSockAddress& localAddress)
    {
        sockFd_ = socket::createTcpSocket(localAddress.ipFamily());
        socket::setSockNonBlock(sockFd_, true);
        for (const auto& channelOption : channelOptions_)
        {
            setChannelOption(channelOption.first, channelOption.second);
        }
        channelOptions_.clear();
        assert(channelOptions_.empty());
        socket::bind(sockFd_, localAddress);
        if (backlog_ > kDefaultMaximumOfBackLog || backlog_ <= 0)
        {
            backlog_ = kDefaultMaximumOfBackLog;
        }
        socket::listen(sockFd_, backlog_);
        addEvent(kReadEvent);
        if (!registerTo())
        {
            THROW_FMT(ChannelRegisterException, "ServerSocketChannel register failed");
        }
    }

    void ServerSocketChannel::setChannelOptions(const ChannelOptionList& channelOptions)
    {
        for (const auto& it : channelOptions)
        {
            channelOptions_.insert_or_assign(it.first, it.second);
        }
    }

    void ServerSocketChannel::setChildOptions(const ChannelOptionList& childOptions)
    {
        for (const auto& it : childOptions)
        {
            childOptions_.insert_or_assign(it.first, it.second);
        }
    }

    void ServerSocketChannel::handleReadEvent()
    {
        assert(eventLoop_->isInCurrentEventLoop());
        InetSockAddress peerAddr {};
        FdType connFd = socket::kInvalidFd;
        if ((connFd = socket::accept(sockFd_, peerAddr)) >= 0)
        {
            NETS_SYSTEM_LOG_DEBUG << "ServerSocketChannel accpet client addr:" << peerAddr.toString();
            InetSockAddress localAddr {};
            socket::getLocalAddress(connFd, localAddr);
            auto socketChannel = ::std::make_shared<SocketChannel>(connFd, localAddr, peerAddr, nextEventLoopFn_());
            initSocketChannel(socketChannel);
            socketChannel->channelActive();
        }
        else
        {
            handleAcceptError(errno);
        }
    }

    void ServerSocketChannel::handleErrorEvent()
    {
        ::int32_t errNum = socket::getSockError(sockFd_);
        THROW_FMT(ServerSocketChannelException, "ServerSocketChannel occurred unexpected exception,errNum=%d", errNum);
    }

    void ServerSocketChannel::initSocketChannel(SocketChannelPtr& socketChannel)
    {
        socketChannel->setChannelOptions(childOptions_);
        for (const auto& childHandler : childHandlers_)
        {
            assert(childHandler.use_count() == 1);
            socketChannel->pipeline().addLast(childHandler);
        }
        if (childInitializationCallback_ != nullptr)
        {
            childInitializationCallback_(*socketChannel);
        }
    }

    void ServerSocketChannel::handleAcceptError(::int32_t errNum)
    {
        switch (errNum)
        {
            // EAGAIN/EWOULDBLOCK is not an error
            // EWOULDBLOCK
            case EAGAIN:
                NETS_SYSTEM_LOG_WARN << "ServerSocketChannel accpet EAGAIN";
                break;
            // allow retry
            case EINTR:
            case EPROTO:
            case ECONNABORTED:
                NETS_SYSTEM_LOG_ERROR << "ServerSocketChannel occurred expected exception while accepting,errno=" << errNum;
                break;
            // the per-process limit on the number of open file descriptors has been reached
            case EMFILE:
            {
                NETS_SYSTEM_LOG_ERROR << "ServerSocketChannel accpet EMFILE";
                socket::dealwithEMFILE(idleFd_, sockFd_);
                break;
            }
            // error
            case EBADF:
            case EFAULT:
            case EINVAL:
            case ENFILE:  // the system-wide limit on the total number of open files has been reached
            case ENOBUFS: // not enough free memory
            case ENOMEM:  // not enough free memory
            case ENOTSOCK:
            case EOPNOTSUPP:
            case EPERM:
            default:
                THROW_FMT(ServerSocketChannelException,
                          "ServerSocketChannel occurred unexpected exception while accepting,errno=%d", errNum);
                break;
        }
    }
} // namespace nets