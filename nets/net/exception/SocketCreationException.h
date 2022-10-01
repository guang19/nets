//
// Created by guang19
//

#ifndef NETS_SOCKET_CREATION_EXCEPTION_H
#define NETS_SOCKET_CREATION_EXCEPTION_H

#include "nets/base/exception/AbstractException.h"

namespace nets
{
    class SocketCreationException : public AbstractException
    {
    public:
        SocketCreationException() : AbstractException() {}
        explicit SocketCreationException(const StringType& cause) : AbstractException(cause) {}
        ~SocketCreationException() override = default;
    };
} // namespace nets

#endif // NETS_SOCKET_CREATION_EXCEPTION_H