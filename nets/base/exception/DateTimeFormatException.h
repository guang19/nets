//
// Created by guang19
//

#ifndef NETS_BASE_DATE_TIME_FORMAT_EXCEPTION_H
#define NETS_BASE_DATE_TIME_FORMAT_EXCEPTION_H

#include "nets/base/exception/AbstractException.h"

namespace nets::base
{
    class DateTimeFormatException : public AbstractException
    {
    public:
        DateTimeFormatException() : AbstractException() {}
        explicit DateTimeFormatException(const StringType& cause) : AbstractException(cause) {}
        ~DateTimeFormatException() override = default;
    };
} // namespace nets::base

#endif // NETS_BASE_DATE_TIME_FORMAT_EXCEPTION_H