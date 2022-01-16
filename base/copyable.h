//
// Created by guang19 on 2021/12/30.
//

#ifndef NETS_BASE_COPYABLE_H
#define NETS_BASE_COPYABLE_H

namespace nets
{
    namespace base
    {
        class copyable
        {
            protected:
                copyable() = default;
                ~copyable() = default;
        };
    } // namespace base
} // namespace nets

#endif // NETS_BASE_COPYABLE_H
