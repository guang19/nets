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
        childOptions_.insert(childOptions_.end(), childOptions.begin(), childOptions.end());
    }

    void ServerSocketChannel::setChannelOptions()
    {
        SockOpt sockOpt = SockOpt::InvalidSockOpt;
        for (const auto& channelOption: channelOptions_)
        {
            sockOpt = channelOption.sockOpt();
            switch (sockOpt)
            {
                case NBACKLOG:
                {
                    backlog_ = ::std::any_cast<int32_t>(channelOption.get());
                    break;
                }
                case NREUSEADDR:
                {
                    socket::setSockReuseAddr(sockFd_, ::std::any_cast<bool>(channelOption.get()));
                    break;
                }
                case NREUSEPORT:
                {
                    socket::setSockReusePort(sockFd_, ::std::any_cast<bool>(channelOption.get()));
                    break;
                }
                case NKEEPALIVE:
                {
                    socket::setSockKeepAlive(sockFd_, ::std::any_cast<bool>(channelOption.get()));
                    break;
                }
                case NTCPNODELAY:
                {
                    socket::setTcpNoDelay(sockFd_, ::std::any_cast<bool>(channelOption.get()));
                    break;
                }
                case NLINGER:
                {
                    auto linger = ::std::any_cast<int32_t>(channelOption.get());
                    socket::setSockLinger(sockFd_, {1, linger});
                    break;
                }
                case NTCPSNDBUF:
                {
                    socket::setSockSendBuf(sockFd_, ::std::any_cast<int32_t>(channelOption.get()));
                    break;
                }
                case NTCPRCVBUF:
                {
                    socket::setSockRecvBuf(sockFd_, ::std::any_cast<int32_t>(channelOption.get()));
                    break;
                }
                case NUDPSNDBUF:
                {
                    socket::setSockSendBuf(sockFd_, ::std::any_cast<int32_t>(channelOption.get()));
                    break;
                }
                case NUDPRCVBUF:
                {
                    socket::setSockRecvBuf(sockFd_, ::std::any_cast<int32_t>(channelOption.get()));
                    break;
                }
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
        setChannelOptions();
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