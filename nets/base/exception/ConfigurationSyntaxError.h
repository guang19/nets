//
// Created by guang19 on 2022/9/3.
//

#ifndef NETS_BASE_CONFIGURATION_SYNTAX_ERROR_H
#define NETS_BASE_CONFIGURATION_SYNTAX_ERROR_H

#include "nets/base/exception/AbstractException.h"

namespace nets::base
{
    class ConfigurationSyntaxError : public AbstractException
    {
    public:
        ConfigurationSyntaxError() : AbstractException() {}
        explicit ConfigurationSyntaxError(const StringType& cause) : AbstractException(cause) {}
        ~ConfigurationSyntaxError() override = default;
    };
} // namespace nets::base

#endif // NETS_BASE_CONFIGURATION_SYNTAX_ERROR_H