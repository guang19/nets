//
// Created by guang19
//

#ifndef NETS_INETSOCK_ADDRESS_EXCEPTION_H
#define NETS_INETSOCK_ADDRESS_EXCEPTION_H

#include "nets/base/exception/AbstractException.h"

namespace nets
{
    class InetSockAddressException : public AbstractException
    {
    public:
        InetSockAddressException() : AbstractException() {}
        explicit InetSockAddressException(const StringType& cause) : AbstractException(cause) {}
        ~InetSockAddressException() override = default;
    };
} // namespace nets

#endif // NETS_INETSOCK_ADDRESS_EXCEPTION_H