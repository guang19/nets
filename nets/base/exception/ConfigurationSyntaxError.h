//
// Created by guang19
//

#ifndef NETS_CONFIGURATION_SYNTAX_ERROR_H
#define NETS_CONFIGURATION_SYNTAX_ERROR_H

#include "nets/base/exception/AbstractException.h"

namespace nets
{
    class ConfigurationSyntaxError : public AbstractException
    {
    public:
        ConfigurationSyntaxError() : AbstractException() {}
        explicit ConfigurationSyntaxError(const StringType& cause) : AbstractException(cause) {}
        ~ConfigurationSyntaxError() override = default;
    };
} // namespace nets

#endif // NETS_CONFIGURATION_SYNTAX_ERROR_H