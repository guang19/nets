//
// Created by guang19 on 2022/7/26.
//

#include "nets/net/core/ChannelOption.h"

namespace nets::net
{
    ::size_t ChannelOption::ChannelOptionHasher::operator()(const ChannelOption& channelOption) const
    {
        return ::std::hash<SockOpt>()(channelOption.sockOpt_) ^ ::std::hash<ValueType>()(channelOption.value_);
    }

    ChannelOption::ChannelOption() : sockOpt_(SockOpt::InvalidSockOpt), value_() {}

    ChannelOption::ChannelOption(SockOpt sockOpt, const ValueType& value) : sockOpt_(sockOpt), value_(value) {}

    ChannelOption::ChannelOption(const ChannelOption& other)
    {
        sockOpt_ = other.sockOpt_;
        value_ = other.value_;
    }

    ChannelOption::ChannelOption(ChannelOption&& other) noexcept
    {
        sockOpt_ = other.sockOpt_;
        value_ = other.value_;
        other.sockOpt_ = SockOpt::InvalidSockOpt;
        other.value_ = {};
    }

    ChannelOption& ChannelOption::operator=(const ChannelOption& other)
    {
        if (this != &other)
        {
            sockOpt_ = other.sockOpt_;
            value_ = other.value_;
        }
        return *this;
    }

    ChannelOption& ChannelOption::operator=(ChannelOption&& other) noexcept
    {
        if (this != &other)
        {
            sockOpt_ = other.sockOpt_;
            value_ = other.value_;
            other.sockOpt_ = SockOpt::InvalidSockOpt;
            other.value_ = {};
        }
        return *this;
    }

    bool ChannelOption::operator==(const ChannelOption& other) const
    {
        return sockOpt_ == other.sockOpt_ && value_ == other.value_;
    }

    bool ChannelOption::operator!=(const ChannelOption& other) const
    {
        return sockOpt_ != other.sockOpt_ || value_ != other.value_;
    }

    bool ChannelOption::operator<(const ChannelOption& other) const
    {
        return sockOpt_ < other.sockOpt_;
    }

    bool ChannelOption::operator>(const ChannelOption& other) const
    {
        return sockOpt_ > other.sockOpt_;
    }

    bool ChannelOption::valid() const
    {
        return sockOpt_ != SockOpt::InvalidSockOpt;
    }

    ::std::any ChannelOption::get() const
    {
        ::std::any any {};
        ::std::visit(
            [&](auto&& item)
            {
                any = item;
            },
            value_);
        return any;
    }

    void ChannelOption::set(const ValueType& value)
    {
        value_ = value;
    }
} // namespace nets::net