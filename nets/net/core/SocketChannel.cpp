//
// Created by guang19 on 2022/5/19.
//

#include "nets/net/core/SocketChannel.h"

#include <cassert>

#include "nets/base/log/Logging.h"
#include "nets/net/core/ByteBuffer.h"

namespace nets::net
{
    SocketChannel::SocketChannel(FdType sockFd, const InetSockAddress& localAddress, const InetSockAddress& peerAddress,
                                 EventLoopRawPtr eventLoop)
        : Channel(eventLoop), sockFd_(sockFd), localAddress_(localAddress), peerAddress_(peerAddress),
          channelHandlerPipeline_(this)
    {
        socket::setSockNonBlock(sockFd_);
    }

    SocketChannel::~SocketChannel()
    {
        socket::closeFd(sockFd_);
    }

    void SocketChannel::setChannelOptions(const ChannelOptionList& channelOptions)
    {
        for (const auto& channelOption: channelOptions)
        {
            setChannelOption(channelOption);
        }
    }

    void SocketChannel::handleReadEvent()
    {
        ByteBuffer byteBuffer(DefaultTcpSockRecvBufSize);
        SSizeType bytes = byteBuffer.writeBytes(*this);
        if (bytes > 0)
        {
            try
            {
                channelHandlerPipeline_.fireChannelRead(byteBuffer);
            }
            catch (const ::std::exception& exception)
            {
                channelHandlerPipeline_.fireExceptionCaught(exception);
            }
        }
        else if (bytes == 0)
        {
        }
    }

    void SocketChannel::handleWriteEvent() {}

    void SocketChannel::handleErrorEvent() {}
} // namespace nets::net