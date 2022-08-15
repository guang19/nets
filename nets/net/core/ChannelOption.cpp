//
// Created by guang19
//

#include "nets/net/core/ChannelOption.h"

namespace nets::net
{
    ::size_t ChannelOption::ChannelOptionHasher::operator()(const ChannelOption& channelOption) const
    {
        return ::std::hash<SockOpt>()(channelOption.sockOpt_) ^ ::std::hash<ValueType>()(channelOption.value_);
    }

    ChannelOption::ChannelOption() : sockOpt_(SockOpt::INVALID_SOCKOPT), value_() {}

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
        other.sockOpt_ = SockOpt::INVALID_SOCKOPT;
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
            other.sockOpt_ = SockOpt::INVALID_SOCKOPT;
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
} // namespace nets::net