//
// Created by guang19
//

#include "nets/base/exception/AbstractException.h"

#include "nets/base/CommonMacro.h"

namespace nets::base
{
    AbstractException::AbstractException() : cause_(), backtraceInfo_(STACK_TRACE) {}

    AbstractException::AbstractException(const StringType& cause) : cause_(cause), backtraceInfo_(STACK_TRACE) {}
} // namespace nets::base