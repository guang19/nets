//
// Created by guang19 on 2022/8/5.
//

#ifndef NETS_BASE_OUT_OF_MEMORY_EXCEPTION_H
#define NETS_BASE_OUT_OF_MEMORY_EXCEPTION_H

#include "nets/base/exception/AbstractException.h"

namespace nets::base
{
    class OutOfMemoryException : public AbstractException
    {
    public:
        OutOfMemoryException() : AbstractException() {}
        explicit OutOfMemoryException(const StringType& cause) : AbstractException(cause) {}
        ~OutOfMemoryException() override = default;
    };
} // namespace nets::base

#endif // NETS_BASE_OUT_OF_MEMORY_EXCEPTION_H
