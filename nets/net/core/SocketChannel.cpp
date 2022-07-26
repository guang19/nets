//
// Created by guang19 on 2022/5/19.
//

#include "nets/net/core/SocketChannel.h"

#include "nets/base/log/Logging.h"

namespace nets::net
{
    SocketChannel::SocketChannel(FdType sockFd, const InetSockAddress& peerAddress, EventLoopRawPtr eventLoop)
        : Channel(eventLoop), sockFd_(sockFd), peerAddress_(peerAddress), channelContext_(this)
    {
        socket::setSockNonBlock(sockFd_);
    }

    void SocketChannel::setChannelOptions(const ChannelOptionList& channelOptions) const
    {
        for (const auto& channelOption: channelOptions)
        {
            const SockOpt sockOpt = channelOption.sockOpt();
            switch (sockOpt)
            {
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
                case NBACKLOG:
                case InvalidSockOpt:
                default:
                    LOGS_ERROR << "SocketChannel set invalid ChannelOption";
                    break;
            }
        }
    }

    void SocketChannel::handleReadEvent() {}

    void SocketChannel::handleWriteEvent() {}

    void SocketChannel::handleErrorEvent() {}
} // namespace nets::net