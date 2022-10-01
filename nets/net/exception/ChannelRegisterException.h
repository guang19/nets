//
// Created by guang19
//

#ifndef NETS_CHANNEL_REGISTER_EXCEPTION_H
#define NETS_CHANNEL_REGISTER_EXCEPTION_H

#include "nets/base/exception/AbstractException.h"

namespace nets
{
    class ChannelRegisterException : public AbstractException
    {
    public:
        ChannelRegisterException() : AbstractException() {}
        explicit ChannelRegisterException(const StringType& cause) : AbstractException(cause) {}
        ~ChannelRegisterException() override = default;
    };
} // namespace nets

#endif // NETS_CHANNEL_REGISTER_EXCEPTION_H