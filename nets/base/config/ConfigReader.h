//
// Created by guang19
//

#ifndef NETS_BASE_CONFIG_READER_H
#define NETS_BASE_CONFIG_READER_H

#include "nets/base/config/Config.h"
#include "nets/base/Singleton.h"

namespace nets::base
{
    DECLARE_SINGLETON_CLASS(ConfigReader)
    {
        DEFINE_SINGLETON(ConfigReader);

    public:
        using StringType = ::std::string;
        using ConfigMap = typename Config::ConfigMap;

    private:
        ConfigReader() = default;
        ~ConfigReader() = default;

    public:
        static Config read(const StringType& file);
    };
} // namespace nets::base

#define CONFIG_READER (nets::base::ConfigReader::getInstance())

#endif // NETS_BASE_CONFIG_READER_H