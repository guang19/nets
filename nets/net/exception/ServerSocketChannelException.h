//
// Created by guang19
//

#ifndef NETS_SERVER_SOCKET_CHANNEL_EXCEPTION_H
#define NETS_SERVER_SOCKET_CHANNEL_EXCEPTION_H

#include "nets/base/exception/AbstractException.h"

namespace nets
{
    class ServerSocketChannelException : public AbstractException
    {
    public:
        ServerSocketChannelException() : AbstractException() {}
        explicit ServerSocketChannelException(const StringType& cause) : AbstractException(cause) {}
        ~ServerSocketChannelException() override = default;
    };
} // namespace nets

#endif // NETS_SERVER_SOCKET_CHANNEL_EXCEPTION_H