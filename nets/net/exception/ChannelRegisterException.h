//
// Created by guang19
//

#ifndef NETS_NET_CHANNEL_REGISTER_EXCEPTION_H
#define NETS_NET_CHANNEL_REGISTER_EXCEPTION_H

#include "nets/base/exception/AbstractException.h"

namespace nets::net
{
    class ChannelRegisterException : public nets::base::AbstractException
    {
    public:
        ChannelRegisterException() : AbstractException() {}
        explicit ChannelRegisterException(const StringType& cause) : AbstractException(cause) {}
        ~ChannelRegisterException() override = default;
    };
} // namespace nets::net

#endif // NETS_NET_CHANNEL_REGISTER_EXCEPTION_H