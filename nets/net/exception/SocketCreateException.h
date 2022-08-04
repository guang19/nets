//
// Created by guang19 on 2022/8/5.
//

#ifndef NETS_NET_SOCKET_CREATE_EXCEPTION_H
#define NETS_NET_SOCKET_CREATE_EXCEPTION_H

#include "nets/base/exception/AbstractException.h"

namespace nets::net
{
    class SocketCreateException : public nets::base::AbstractException
    {
    public:
        SocketCreateException() : AbstractException() {}
        explicit SocketCreateException(const StringType& cause) : AbstractException(cause) {}
        ~SocketCreateException() override = default;
    };
} // namespace nets::net

#endif // NETS_NET_SOCKET_CREATE_EXCEPTION_H
