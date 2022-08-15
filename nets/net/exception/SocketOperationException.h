//
// Created by guang19
//

#ifndef NETS_NET_SOCKET_OPERATION_EXCEPTION_H
#define NETS_NET_SOCKET_OPERATION_EXCEPTION_H

#include "nets/base/exception/AbstractException.h"

namespace nets::net
{
    class SocketOperationException : public nets::base::AbstractException
    {
    public:
        SocketOperationException() : AbstractException() {}
        explicit SocketOperationException(const StringType& cause) : AbstractException(cause) {}
        ~SocketOperationException() override = default;
    };
} // namespace nets::net

#endif // NETS_NET_SOCKET_OPERATION_EXCEPTION_H