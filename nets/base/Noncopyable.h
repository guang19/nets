//
// Created by guang19
//

#ifndef NETS_NONCOPYABLE_H
#define NETS_NONCOPYABLE_H

namespace nets
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
} // namespace nets

#endif // NETS_NONCOPYABLE_H