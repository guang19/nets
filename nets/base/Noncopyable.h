//
// Created by guang19
//

#ifndef NETS_BASE_NONCOPYABLE_H
#define NETS_BASE_NONCOPYABLE_H

namespace nets::base
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
} // namespace nets::base

#endif // NETS_BASE_NONCOPYABLE_H