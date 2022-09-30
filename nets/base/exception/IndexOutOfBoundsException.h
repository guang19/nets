//
// Created by guang19
//

#ifndef NETS_BASE_INDEX_OUT_OF_BOUNDS_EXCEPTION_H
#define NETS_BASE_INDEX_OUT_OF_BOUNDS_EXCEPTION_H

#include "nets/base/exception/AbstractException.h"

namespace nets::base
{
    class IndexOutOfBoundsException : public AbstractException
    {
    public:
        IndexOutOfBoundsException() : AbstractException() {}
        explicit IndexOutOfBoundsException(const StringType& cause) : AbstractException(cause) {}
        ~IndexOutOfBoundsException() override = default;
    };
} // namespace nets::base

#endif // NETS_BASE_INDEX_OUT_OF_BOUNDS_EXCEPTION_H