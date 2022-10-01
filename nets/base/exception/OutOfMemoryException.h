//
// Created by guang19
//

#ifndef NETS_OUT_OF_MEMORY_EXCEPTION_H
#define NETS_OUT_OF_MEMORY_EXCEPTION_H

#include "nets/base/exception/AbstractException.h"

namespace nets
{
    class OutOfMemoryException : public AbstractException
    {
    public:
        OutOfMemoryException() : AbstractException() {}
        explicit OutOfMemoryException(const StringType& cause) : AbstractException(cause) {}
        ~OutOfMemoryException() override = default;
    };
} // namespace nets

#endif // NETS_OUT_OF_MEMORY_EXCEPTION_H