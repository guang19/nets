//
// Created by guang19
//

#ifndef NETS_INDEX_OUT_OF_BOUNDS_EXCEPTION_H
#define NETS_INDEX_OUT_OF_BOUNDS_EXCEPTION_H

#include "nets/base/exception/AbstractException.h"

namespace nets
{
    class IndexOutOfBoundsException : public AbstractException
    {
    public:
        IndexOutOfBoundsException() : AbstractException() {}
        explicit IndexOutOfBoundsException(const StringType& cause) : AbstractException(cause) {}
        ~IndexOutOfBoundsException() override = default;
    };
} // namespace nets

#endif // NETS_INDEX_OUT_OF_BOUNDS_EXCEPTION_H