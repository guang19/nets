//
// Created by guang19
//

#include "nets/base/config/ConfigReader.h"

namespace nets::base
{
    INIT_SINGLETON(ConfigReader);

    Config ConfigReader::read(const StringType& file)
    {
        ConfigMap config {};
        
        return Config(config);
    }
}