//
// Created by guang19
//

#ifndef NETS_BASE_COPYABLE_H
#define NETS_BASE_COPYABLE_H

namespace nets::base
{
    class Copyable
    {
    protected:
        Copyable() = default;
        ~Copyable() = default;
    };
} // namespace nets::base

#endif // NETS_BASE_COPYABLE_H