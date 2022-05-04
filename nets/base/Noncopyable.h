//
// Created by guang19 on 2021/12/30.
//

#ifndef NETS_BASE_NONCOPYABLE_H
#define NETS_BASE_NONCOPYABLE_H

namespace nets
{
    namespace base
    {
        class Noncopyable
        {
            protected:
                Noncopyable() = default;
                ~Noncopyable() = default;

            public:
                Noncopyable(const Noncopyable&) = delete;
                Noncopyable(Noncopyable&&) = delete;
				Noncopyable& operator=(const Noncopyable&) = delete;
				Noncopyable& operator=(Noncopyable&&) = delete;
        };
    } // namespace base
} // namespace nets

#endif // NETS_BASE_NONCOPYABLE_H