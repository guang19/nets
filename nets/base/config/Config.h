//
// Created by guang19
//

#ifndef NETS_BASE_CONFIG_H
#define NETS_BASE_CONFIG_H

#include <map>
#include <string>

#include "nets/base/Copyable.h"

namespace nets::base
{
    class Config : public Copyable
    {
    public:
        using StringType = ::std::string;
        using IntType = ::int32_t;
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
        StringType getString(const StringType& key, const StringType& defaultValue = "");
        IntType getInt(const StringType& key, IntType defaultValue = 0);
        bool getBool(const StringType& key, bool defaultValue = false);

    private:
        ConfigMap config_ {};
    };
}

#endif // NETS_BASE_CONFIG_H