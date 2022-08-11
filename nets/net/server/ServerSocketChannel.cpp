//
// Created by guang19 on 2022/5/18.
//

#include "nets/net/server/ServerSocketChannel.h"

#include <cassert>

#include "nets/base/log/Logging.h"
#include "nets/net/core/EventLoop.h"
#include "nets/net/exception/ChannelRegisterException.h"
#include "nets/net/exception/ServerSocketChannelException.h"

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
        // default options
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

    FdType ServerSocketChannel::fd() const
    {
        return sockFd_;
    }

    void ServerSocketChannel::handleReadEvent()
    {
        assert(eventLoop_->isInCurrentEventLoop());
        InetSockAddress peerAddr {};
        FdType connFd = socket::InvalidFd;
        if ((connFd = socket::accept(sockFd_, peerAddr.sockAddr6())) >= 0)
        {
            LOGS_DEBUG << "ServerSocketChannel accpet client addr:" << peerAddr.toString();
            InetSockAddress localAddr {};
            socket::getLocalAddress(connFd, localAddr.sockAddr6());
            auto socketChannel = ::std::make_shared<SocketChannel>(connFd, localAddr, peerAddr, nextEventLoopFn_());
            initSocketChannel(socketChannel);
        }
        else
        {
            int32_t errNum = errno;
            handleAcceptError(errNum);
        }
    }

    void ServerSocketChannel::handleErrorEvent()
    {
        int32_t errNum = socket::getSockError(sockFd_);
        THROW_FMT(ServerSocketChannelException, "ServerSocketChannel occurred unexpected exception,errNum=%d", errNum);
    }

    void ServerSocketChannel::bind(const InetSockAddress& sockAddress)
    {
        sockFd_ = socket::createTcpSocket(sockAddress.ipFamily());
        socket::setSockNonBlock(sockFd_, true);
        for (const auto& channelOption: channelOptions_)
        {
            setChannelOption(channelOption);
        }
        socket::bind(sockFd_, sockAddress.sockAddr());
        socket::listen(sockFd_, backlog_);
        addEvent(EReadEvent);
        if (!registerTo())
        {
            THROW_FMT(ChannelRegisterException, "ServerSocketChannel register failed");
        }
    }

    void ServerSocketChannel::initSocketChannel(SocketChannelPtr& socketChannel)
    {
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
        socketChannel->channelActive();
    }

    void ServerSocketChannel::handleAcceptError(int32_t errNum)
    {
        switch (errNum)
        {
            // EAGAIN/EWOULDBLOCK is not an error
            // EWOULDBLOCK
            case EAGAIN:
                LOGS_WARN << "ServerSocketChannel accpet EAGAIN";
                break;
            case EINTR:
            case EPROTO:
            case ECONNABORTED:
                LOGS_ERROR << "ServerSocketChannel occurred expected exception occurred while accepting,errno=" << errNum;
                break;
            // the per-process limit on the number of open file descriptors has been reached
            case EMFILE:
            {
                LOGS_ERROR << "ServerSocketChannel accpet EMFILE";
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
} // namespace nets::net