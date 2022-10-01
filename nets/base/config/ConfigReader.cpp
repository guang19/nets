//
// Created by guang19
//

#include "nets/base/config/ConfigReader.h"

#include <fstream>

#include "nets/base/exception/ConfigurationSyntaxError.h"
#include "nets/base/StringUtils.h"

namespace nets
{
    INIT_SINGLETON(ConfigReader);

    Config ConfigReader::read(const StringType& file)
    {
        ::std::ifstream fileStream(file.c_str(), ::std::ios::in);
        if (!fileStream.good())
        {
            THROW_FILE_OPEN_EXCEPTION(errno)
        }
        ConfigMap config {};
        while (!fileStream.eof() && fileStream.good())
        {
            StringType line {};
            ::std::getline(fileStream, line);
            utils::trim(line);
            if (!line.empty())
            {
                if (line[0] == '#' || line[0] == ';')
                {
                    continue;
                }
                StringType::size_type spaceIndex = line.find(' ');
                if (spaceIndex == StringType::npos)
                {
                    THROW_FMT(ConfigurationSyntaxError, "configuration syntax error,the error statement is %s",
                              line.c_str());
                }
                StringType key = line.substr(0, spaceIndex);
                StringType value = line.substr(spaceIndex + 1);
                config.insert_or_assign(key, value);
            }
        }
        return Config(config);
    }
} // namespace nets