// MIT License
//
// Copyright (c) 2022 guang19
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

// @brief String utils

#include "nets/base/StringUtils.h"

#include <algorithm>
#include <cwctype>

namespace nets::utils
{
    void trim(StringType& str)
    {
        if (!str.empty())
        {
            for (auto it = str.begin(); it != str.end();)
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

    void split(const StringType& s, std::vector<StringType>& tokens, const StringType& delimiter)
    {
        if (s.length() == 0)
        {
            return;
        }
        SizeType pos1 = 0;
        SizeType pos2 = s.find(delimiter);
        while(StringType::npos != pos2)
        {
            tokens.push_back(s.substr(pos1, pos2 - pos1));
            pos1 = pos2 + delimiter.length();
            pos2 = s.find(delimiter, pos1);
        }
        if(pos1 != s.length())
        {
            tokens.push_back(s.substr(pos1));
        }
    }

    struct AsciiCaseInsensitive
    {
        bool operator()(char lhs, char rhs) const
        {
            char k = lhs ^ rhs;
            if (k == 0)
            {
                return true;
            }
            if (k != 32)
            {
                return false;
            }
            k = lhs | rhs;
            return (k >= 'a' && k <= 'z');
        }
    };

    bool caseInsensitiveEqual(const StringType& s1, const StringType& s2)
    {
        if (s1.length() != s2.length())
        {
            return false;
        }
        else
        {
            return ::std::equal(s1.begin(), s1.end(), s2.begin(), AsciiCaseInsensitive());
        }
    }
} // namespace nets::utils