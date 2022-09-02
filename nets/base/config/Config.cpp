//
// Created by guang19
//

#include "nets/base/config/Config.h"

namespace nets::base
{
    Config::Config(const ConfigMap& config) : config_(config) {}

    Config::Config(const Config& other) : config_(other.config_) {}

    Config::Config(Config&& other) noexcept : config_(::std::move(other.config_)) {}

    Config& Config::operator=(const Config& other)
    {
        if (this != &other)
        {
            config_ = other.config_;
        }
        return *this;
    }

    Config& Config::operator=(Config&& other) noexcept
    {
        if (this != &other)
        {
            config_ = ::std::move(other.config_);
        }
        return *this;
    }

    void Config::swap(Config&& other)
    {
        ::std::swap(config_, other.config_);
    }

    Config::StringType Config::getString(const StringType& key, const StringType& defaultValue)
    {
        auto it = config_.find(key);
        if (it != config_.end())
        {
            return it->second;
        }
        return defaultValue;
    }

    Config::IntType Config::getInt(const StringType& key, IntType defaultValue)
    {
        auto it = config_.find(key);
        if (it != config_.end())
        {
            return ::std::stoi(it->second);
        }
        return defaultValue;
    }

    bool Config::getBool(const StringType& key, bool defaultValue)
    {
        auto it = config_.find(key);
        if (it != config_.end())
        {
            if (it->second == "true")
            {
                return true;
            }
            else
            {
                return false;
            }
        }
        return defaultValue;
    }
} // namespace nets::base