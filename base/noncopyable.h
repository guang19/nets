//
// Created by guang19 on 2021/12/30.
//

#ifndef NETS_BASE_NONCOPYABLE_H
#define NETS_BASE_NONCOPYABLE_H

namespace nets
{
    namespace base
    {
        class noncopyable
        {
            protected:
                noncopyable() = default;
                ~noncopyable() = default;

            public:
                noncopyable(const noncopyable&) = delete;
                noncopyable(noncopyable&&) = delete;
                void operator=(const noncopyable&) = delete;
                void operator=(noncopyable&&) = delete;
        };
    } // namespace base
} // namespace nets

#endif // NETS_BASE_NONCOPYABLE_H
