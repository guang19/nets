//
// Created by guang19
//

#ifndef NETS_TYPES_H
#define NETS_TYPES_H

#include <cstdint>
#include <string>

namespace nets
{
    using StringType = ::std::string;
    using StringViewType = ::std::string_view;
    using SizeType = ::size_t;
    using TimeType = ::time_t;
    using Int8Type = ::int8_t;
    using Uint8Type = ::uint8_t;
    using Int16Type = ::int16_t;
    using Uint16Type = ::uint16_t;
    using Int32Type = ::int32_t;
    using Uint32Type = ::uint32_t;
    using Int64Type = ::int64_t;
    using Uint64Type = ::uint64_t;
    using IntPtrType = ::intptr_t;
    using UintPtrType = ::uintptr_t;
} // namespace nets

#endif // NETS_TYPES_H