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
#include "nets/base/StringUtils.h"

namespace nets
{
    namespace
    {
        const char* kHttpMethodName[] = {"CONNECT", "DELETE", "GET", "HEAD", "OPTIONS", "PATCH", "POST", "PUT", "TRACE"};
        const char* kHttpProtocolVersionName[] = {"HTTP/1.0", "HTTP/1.1"};
        const char* kHttpHeaderName[] = {"Accept",
                                         "Accept-CH",
                                         "Accept-CH-Lifetime",
                                         "Accept-Charset",
                                         "Accept-Encoding",
                                         "Accept-Language",
                                         "Accept-Patch",
                                         "Accept-Post",
                                         "Accept-Ranges",
                                         "Access-Control-Allow-Credentials",
                                         "Access-Control-Allow-Headers",
                                         "Access-Control-Allow-Methods",
                                         "Access-Control-Allow-Origin",
                                         "Access-Control-Expose-Headers",
                                         "Access-Control-Max-Age",
                                         "Access-Control-Request-Headers",
                                         "Access-Control-Request-Method",
                                         "Age",
                                         "Allow",
                                         "Alt-Svc",
                                         "Authorization",
                                         "Cache-Control",
                                         "Clear-Site-Data",
                                         "Connection",
                                         "Content-Disposition",
                                         "Content-DPR",
                                         "Content-Encoding",
                                         "Content-Language",
                                         "Content-Length",
                                         "Content-Location",
                                         "Content-Range",
                                         "Content-Security-Policy",
                                         "Content-Security-Policy-Report-Only",
                                         "Content-Type",
                                         "Cookie",
                                         "Cross-Origin-Embedder-Policy",
                                         "Cross-Origin-Opener-Policy",
                                         "Cross-Origin-Resource-Policy",
                                         "Date",
                                         "Device-Memory",
                                         "Digest",
                                         "DNT",
                                         "Downlink",
                                         "DPR",
                                         "Early-Data",
                                         "ECT",
                                         "ETag",
                                         "Expect",
                                         "Expect-CT",
                                         "Expires",
                                         "Feature-Policy",
                                         "Forwarded",
                                         "From",
                                         "Host",
                                         "If-Match",
                                         "If-Modified-Since",
                                         "If-None-Match",
                                         "If-Range",
                                         "If-Unmodified-Since",
                                         "Keep-Alive",
                                         "Large-Allocation",
                                         "Last-Modified",
                                         "Link",
                                         "Location",
                                         "Max-Forwards",
                                         "NEL",
                                         "Origin",
                                         "Pragma",
                                         "Proxy-Authenticate",
                                         "Proxy-Authorization",
                                         "Range",
                                         "Referer",
                                         "Referrer-Policy",
                                         "Retry-After",
                                         "RTT",
                                         "Save-Data",
                                         "Sec-CH-UA",
                                         "Sec-CH-UA-Arch",
                                         "Sec-CH-UA-Bitness",
                                         "Sec-CH-UA-Full-Version",
                                         "Sec-CH-UA-Full-Version-List",
                                         "Sec-CH-UA-Mobile",
                                         "Sec-CH-UA-Model",
                                         "Sec-CH-UA-Platform",
                                         "Sec-CH-UA-Platform-Version",
                                         "Sec-Fetch-Dest",
                                         "Sec-Fetch-Mode",
                                         "Sec-Fetch-Site",
                                         "Sec-Fetch-User",
                                         "Sec-GPC",
                                         "Sec-WebSocket-Accept",
                                         "Server",
                                         "Server-Timing",
                                         "Service-Worker-Navigation-Preload",
                                         "Set-Cookie",
                                         "SourceMap",
                                         "Strict-Transport-Security",
                                         "TE",
                                         "Timing-Allow-Origin",
                                         "Tk",
                                         "Trailer",
                                         "Transfer-Encoding",
                                         "Upgrade",
                                         "Upgrade-Insecure-Requests",
                                         "User-Agent",
                                         "Vary",
                                         "Via",
                                         "Viewport-Width",
                                         "Want-Digest",
                                         "Warning",
                                         "Width",
                                         "WWW-Authenticate",
                                         "X-Content-Type-Options",
                                         "X-DNS-Prefetch-Control",
                                         "X-Forwarded-For",
                                         "X-Forwarded-Host",
                                         "X-Forwarded-Proto",
                                         "X-Frame-Options",
                                         "X-XSS-Protection"};
    } // namespace

    StringType httpMethodToString(HttpMethod method)
    {
        return kHttpMethodName[ENUM_CLASS_TO_INT(method)];
    }

    HttpMethod stringToHttpMethod(const StringType& methodStr)
    {
        constexpr Int32Type kNumOfHttpMethod = sizeof(kHttpMethodName);
        for (Int32Type i = 0; i < kNumOfHttpMethod; ++i)
        {
            if (utils::caseInsensitiveEqual(methodStr, kHttpMethodName[i]))
            {
                return HttpMethod(i);
            }
        }
        return HttpMethod::UNKNOWN;
    }

    StringType httpProtocolVersionToString(HttpProtocolVersion protocolVersion)
    {
        return kHttpProtocolVersionName[ENUM_CLASS_TO_INT(protocolVersion)];
    }

    HttpProtocolVersion stringToHttpProtocolVersion(const StringType& protocolVersionStr)
    {
        constexpr Int32Type kNumOfHttpProtocolVersion = sizeof(kHttpProtocolVersionName);
        for (Int32Type i = 0; i < kNumOfHttpProtocolVersion; ++i)
        {
            if (utils::caseInsensitiveEqual(protocolVersionStr, kHttpProtocolVersionName[i]))
            {
                return HttpProtocolVersion(i);
            }
        }
        return HttpProtocolVersion::UNSUPPORTED;
    }

    StringType httpHeaderToString(HttpHeader httpHeader)
    {
        return kHttpHeaderName[ENUM_CLASS_TO_INT(httpHeader)];
    }
} // namespace nets