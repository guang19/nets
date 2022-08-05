//
// Created by guang19 on 2022/8/5.
//

#ifndef NETS_NET_SOCKET_CREATION_EXCEPTION_H
#define NETS_NET_SOCKET_CREATION_EXCEPTION_H

#include "nets/base/exception/AbstractException.h"

namespace nets::net
{
    class SocketCreationException : public nets::base::AbstractException
    {
    public:
        SocketCreationException() : AbstractException() {}
        explicit SocketCreationException(const StringType& cause) : AbstractException(cause) {}
        ~SocketCreationException() override = default;
    };
} // namespace nets::net

#endif // NETS_NET_SOCKET_CREATION_EXCEPTION_H
