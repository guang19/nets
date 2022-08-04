//
// Created by guang19 on 2022/8/5.
//

#ifndef NETS_NET_INETSOCK_ADDRESS_EXCEPTION_H
#define NETS_NET_INETSOCK_ADDRESS_EXCEPTION_H

#include "nets/base/exception/AbstractException.h"

namespace nets::net
{
    class InetSockAddressException : public nets::base::AbstractException
    {
    public:
        InetSockAddressException() : AbstractException() {}
        explicit InetSockAddressException(const StringType& cause) : AbstractException(cause) {}
        ~InetSockAddressException() override = default;
    };
} // namespace nets::net

#endif // NETS_NET_INETSOCK_ADDRESS_EXCEPTION_H
