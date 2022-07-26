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
            LOGS_FATAL << "ServerSocketChannel createIdleFd failed";
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
        childOptions_.insert(channelOptions_.end(), childOptions.begin(), childOptions.end());
    }

    void ServerSocketChannel::setChannelOptions()
    {
        for (const auto& channelOption : channelOptions_)
        {
            const SockOpt sockOpt = channelOption.sockOpt();
            switch (sockOpt)
            {
                case NBACKLOG:
                    if (channelOption != NBackLog)
                    {
                        backlog_ = ::std::any_cast<int32_t>(channelOption.get());
                    }
                    break;
                case NREUSEADDR:
                    if (channelOption != NReuseAddr)
                    {
                        socket::setSockReuseAddr(sockFd_, ::std::any_cast<bool>(channelOption.get()));
                    }
                    break;
                case NREUSEPORT:
                    if (channelOption != NReusePort)
                    {
                        socket::setSockReusePort(sockFd_, ::std::any_cast<bool>(channelOption.get()));
                    }
                    break;
                case NKEEPALIVE:
                    if (channelOption != NKeepAlive)
                    {
                        socket::setSockKeepAlive(sockFd_, ::std::any_cast<bool>(channelOption.get()));
                    }
                    break;
                case NTCPNODELAY:
                    if (channelOption != NTcpNoDelay)
                    {
                        socket::setTcpNoDelay(sockFd_, ::std::any_cast<bool>(channelOption.get()));
                    }
                    break;
                case NLINGER:
                    if (channelOption != NLinger)
                    {
                        auto linger = ::std::any_cast<int32_t>(channelOption.get());
                        socket::setSockLinger(sockFd_, {1, linger});
                    }
                    break;
                case NTCPSNDBUF:
                    if (channelOption != NTcpSendBuf)
                    {
                        socket::setSockSendBuf(sockFd_, ::std::any_cast<int32_t>(channelOption.get()));
                    }
                    break;
                case NTCPRCVBUF:
                    if (channelOption != NTcpRecvBuf)
                    {
                        socket::setSockRecvBuf(sockFd_, ::std::any_cast<int32_t>(channelOption.get()));
                    }
                    break;
                case NUDPSNDBUF:
                    if (channelOption != NUdpSendBuf)
                    {
                        socket::setSockSendBuf(sockFd_, ::std::any_cast<int32_t>(channelOption.get()));
                    }
                    break;
                case NUDPRCVBUF:
                    if (channelOption != NUdpRecvBuf)
                    {
                        socket::setSockRecvBuf(sockFd_, ::std::any_cast<int32_t>(channelOption.get()));
                    }
                    break;
                case InvalidSockOpt:
                default:
                    LOGS_ERROR << "Channel set invalid ChannelOption";
                    break;
            }
        }
    }

    void ServerSocketChannel::bind(const InetSockAddress& sockAddress)
    {
        sockFd_ = socket::createTcpSocket(sockAddress.ipFamily());
        socket::setSockNonBlock(sockFd_, true);
        socket::bind(sockFd_, sockAddress.csockAddr());
        socket::listen(sockFd_, backlog_);
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
            clientSocketChannel->setChannelOptions(childOptions_);
            for (const auto& childHandler: childHandlers_)
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