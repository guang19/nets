//
// Created by guang19
//

#ifndef NETS_NET_SOCKET_CHANNEL_EXCEPTION_H
#define NETS_NET_SOCKET_CHANNEL_EXCEPTION_H

#include "nets/base/exception/AbstractException.h"

namespace nets::net
{
    class SocketChannelException : public nets::base::AbstractException
    {
    public:
        SocketChannelException() : AbstractException() {}
        explicit SocketChannelException(const StringType& cause) : AbstractException(cause) {}
        ~SocketChannelException() override = default;
    };
} // namespace nets::net

#endif // NETS_NET_SOCKET_CHANNEL_EXCEPTION_H