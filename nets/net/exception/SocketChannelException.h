//
// Created by guang19
//

#ifndef NETS_SOCKET_CHANNEL_EXCEPTION_H
#define NETS_SOCKET_CHANNEL_EXCEPTION_H

#include "nets/base/exception/AbstractException.h"

namespace nets
{
    class SocketChannelException : public AbstractException
    {
    public:
        SocketChannelException() : AbstractException() {}
        explicit SocketChannelException(const StringType& cause) : AbstractException(cause) {}
        ~SocketChannelException() override = default;
    };
} // namespace nets

#endif // NETS_SOCKET_CHANNEL_EXCEPTION_H