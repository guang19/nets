//
// Created by guang19
//

#include "nets/base/StringUtils.h"

#include <cwctype>

namespace nets::base::utils
{
    void trim(StringType& str)
    {
        if (!str.empty())
        {
            for (auto it = str.begin(); it != str.end(); ++it)
            {
                if (!::iswspace(*it))
                {
                    break;
                }
                str.erase(it);
            }
            for (auto it = str.end(); it != str.begin();)
            {
                if (!::iswspace(*(--it)))
                {
                    break;
                }
                str.erase(it);
            }
        }
    }
} // namespace nets::base::utils