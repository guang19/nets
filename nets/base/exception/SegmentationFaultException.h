//
// Created by guang19
//

#ifndef NETS_SEGMENTATION_FAULT_EXCEPTION_H
#define NETS_SEGMENTATION_FAULT_EXCEPTION_H

#include "nets/base/exception/AbstractException.h"

namespace nets
{
    class SegmentationFaultException : public AbstractException
    {
    public:
        SegmentationFaultException() : AbstractException() {}
        explicit SegmentationFaultException(const StringType& cause) : AbstractException(cause) {}
        ~SegmentationFaultException() override = default;
    };
} // namespace nets

#endif // NETS_SEGMENTATION_FAULT_EXCEPTION_H