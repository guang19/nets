//
// Created by guang19
//

#ifndef NETS_CONFIG_READER_H
#define NETS_CONFIG_READER_H

#include "nets/base/config/Config.h"
#include "nets/base/Singleton.h"

namespace nets
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
        Config read(const StringType& file);
    };
} // namespace nets

#define CONFIG_READER (nets::ConfigReader::getInstance())

#endif // NETS_CONFIG_READER_H