//
// Created by guang19
//

#ifndef NETS_SOCKET_OPERATION_EXCEPTION_H
#define NETS_SOCKET_OPERATION_EXCEPTION_H

#include "nets/base/exception/AbstractException.h"

namespace nets
{
    class SocketOperationException : public AbstractException
    {
    public:
        SocketOperationException() : AbstractException() {}
        explicit SocketOperationException(const StringType& cause) : AbstractException(cause) {}
        ~SocketOperationException() override = default;
    };
} // namespace nets

#endif // NETS_SOCKET_OPERATION_EXCEPTION_H