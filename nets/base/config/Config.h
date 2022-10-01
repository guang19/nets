//
// Created by guang19
//

#ifndef NETS_CONFIG_H
#define NETS_CONFIG_H

#include <map>
#include <string>

#include "nets/base/Copyable.h"

namespace nets
{
    // k-v configuration
    class Config : public Copyable
    {
    public:
        using IntType = ::int32_t;
        using LongType = ::int64_t;
        using StringType = ::std::string;
        using ConfigMap = ::std::map<StringType, StringType>;

    public:
        explicit Config(const ConfigMap& config);
        ~Config() = default;

        Config(const Config& other);
        Config(Config&& other) noexcept;
        Config& operator=(const Config& other);
        Config& operator=(Config&& other) noexcept;

        void swap(Config&& other);

    public:
        inline bool isEmpty() const
        {
            return config_.empty();
        }

        StringType getString(const StringType& key, const StringType& defaultValue = "");
        IntType getInt(const StringType& key, IntType defaultValue = 0);
        LongType getLong(const StringType& key, LongType defaultValue = 0L);
        bool getBool(const StringType& key, bool defaultValue = false);

    private:
        ConfigMap config_ {};
    };
} // namespace nets

#endif // NETS_CONFIG_H