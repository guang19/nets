//
// Created by n021949 on 2022/8/5.
//

#ifndef NETS_NET_SERVER_SOCKET_CHANNEL_EXCEPTION_H
#define NETS_NET_SERVER_SOCKET_CHANNEL_EXCEPTION_H

#include "nets/base/exception/AbstractException.h"

namespace nets::net
{
    class ServerSocketChannelException : public nets::base::AbstractException
    {
    public:
        ServerSocketChannelException() : AbstractException() {}
        explicit ServerSocketChannelException(const StringType& cause) : AbstractException(cause) {}
        ~ServerSocketChannelException() override = default;
    };
} // namespace nets::net

#endif // NETS_NET_SERVER_SOCKET_CHANNEL_EXCEPTION_H