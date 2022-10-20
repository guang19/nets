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

// @brief Http common file

#include "nets/protocol/http/HttpCommon.h"

#include "nets/base/Common.h"

namespace nets
{
    namespace
    {
        const StringType kHttpMethodString[] = {
            "CONNECT", "DELETE", "GET", "HEAD", "OPTIONS", "PATCH", "POST", "PUT", "TRACE"};
        const StringType kHttpProtocolVersionString[] = {"HTTP/1.0", "HTTP/1.1"};
    }

    StringType methodToString(HttpMethod method)
    {
        return kHttpMethodString[ENUM_CLASS_TO_INT(method)];
    }

    HttpMethod stringToMethod(const StringType& methodStr)
    {
        constexpr Int32Type kNumOfHttpMethod = sizeof(kHttpMethodString);
        for (Int32Type i = 0; i < kNumOfHttpMethod; ++i)
        {
            if (caseInsensitiveEqual(methodStr, kHttpMethodString[i]))
            {
                return HttpMethod(i);
            }
        }
        return HttpMethod::UNKNOWN;
    }

    StringType protocolVersionToString(HttpProtocolVersion protocolVersion)
    {
        return kHttpProtocolVersionString[ENUM_CLASS_TO_INT(protocolVersion)];
    }

    HttpProtocolVersion stringToProtocolVersion(const StringType& protocolVersionStr)
    {
        constexpr Int32Type kNumOfHttpProtocolVersion = sizeof(kHttpProtocolVersionString);
        for (Int32Type i = 0; i < kNumOfHttpProtocolVersion; ++i)
        {
            if (caseInsensitiveEqual(protocolVersionStr, kHttpProtocolVersionString[i]))
            {
                return HttpProtocolVersion(i);
            }
        }
        return HttpProtocolVersion::UNSUPPORTED;
    }
} // namespace nets