//
// Created by guang19 on 2022/5/19.
//

#include "nets/net/core/SocketChannel.h"

#include "nets/base/log/Logging.h"
#include "nets/net/core/EventLoop.h"
#include "nets/net/exception/SocketChannelException.h"

namespace nets::net
{
    SocketChannel::SocketChannel(FdType sockFd, const InetSockAddress& localAddress, const InetSockAddress& peerAddress,
                                 EventLoopRawPtr eventLoop)
        : Channel(eventLoop), sockFd_(sockFd), localAddress_(localAddress), peerAddress_(peerAddress), sendBuffer_(),
          channelHandlerPipeline_(this)
    {
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
            // passively closed, the client may shutdown(write) or close the socket. if the client shutdown write, the tcp
            // connection is half-close state, the server can still send data, and the client can still recv it; if the
            // client closes the socket, the server will also close the socket. if the server still needs to send data after
            // the client shutdown write, then let the user send data in the disConnect callback, and the server will close
            // the socket after disConnect
            try
            {
                channelHandlerPipeline_.fireChannelDisconnect();
            }
            catch (const ::std::exception& exception)
            {
                channelHandlerPipeline_.fireExceptionCaught(exception);
            }
            // if cancel directly, because the channel is destroyed in advance, an exception may be thrown when other
            // events of the channel are executed in the current loop. so add task to pendingTasksQueue, wait next loop execute
            eventLoop_->addTask(
                [channel = shared_from_this()]()
                {
                    channel->deregister();
                    assert(channel.use_count() == 1);
                });
        }
        else
        {
            int32_t errNum = errno;
            handleReadError(errNum);
        }
    }

    void SocketChannel::handleReadError(int32_t errNum)
    {
        switch (errNum)
        {
            // EAGAIN/EWOULDBLOCK is not an error
            // EWOULDBLOCK
            case EAGAIN:
            case EINTR:
                LOGS_ERROR << "SocketChannel read expected exception,errno=" << errNum;
                break;
            // error
            case EBADF:
            case EFAULT:
            case EINVAL:
            case EIO:
            case EPERM:
            case EISDIR:
            case ENOMEM:
            case ENOTCONN:
            case ENOTSOCK:
            default:
                errno = errNum;
                LOGS_ERROR << "SocketChannel read unexpected exception,errno=" << errNum;
                break;
        }
    }

    void SocketChannel::handleWriteEvent()
    {
        LOGS_DEBUG << "SocketChannel::handleWriteEvent";
        int32_t n = socket::write(sockFd_, "haha", 4);
        LOGS_DEBUG << "write " << n << " bytes";
    }

    void SocketChannel::handleErrorEvent()
    {
        int32_t errNum = socket::getSockError(sockFd_);
        LOGS_ERROR << "SocketChannel unexpected exception,errNum=" << errNum;
    }
} // namespace nets::net