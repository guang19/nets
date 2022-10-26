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

    HttpStatus statusCodeToHttpStatus(Int32Type statusCode)
    {
        return HttpStatus(statusCode);
    }

    Int32Type httpStatusToCode(HttpStatus httpStatus)
    {
        return ENUM_CLASS_TO_INT(httpStatus);
    }

    StringType httpStatusToText(HttpStatus httpStatus)
    {
        switch (httpStatus)
        {
                // 1xx
            case HttpStatus::CONTINUE:
                return "Continue";
            case HttpStatus::SWITCHING_PROTOCOLS:
                return "Switching Protocols";
            case HttpStatus::EARLY_HINTS:
                return "Early Hints";

                // 2xx
            case HttpStatus::OK:
                return "OK";
            case HttpStatus::CREATED:
                return "Created";
            case HttpStatus::ACCEPTED:
                return "Accepted";
            case HttpStatus::NON_AUTHORITATIVE_INFORMATION:
                return "Non-Authoritative Information";
            case HttpStatus::NO_CONTENT:
                return "No Content";
            case HttpStatus::RESET_CONTENT:
                return "Reset Content";
            case HttpStatus::PARTIAL_CONTENT:
                return "Partial Content";

                // 3xx
            case HttpStatus::MULTIPLE_CHOICES:
                return "Multiple Choices";
            case HttpStatus::MOVED_PERMANENTLY:
                return "Moved Permanently";
            case HttpStatus::FOUND:
                return "Found";
            case HttpStatus::SEE_OTHER:
                return "See Other";
            case HttpStatus::NOT_MODIFIED:
                return "Not Modified";
            case HttpStatus::TEMPORARY_REDIRECT:
                return "Temporary Redirect";
            case HttpStatus::PERMANENT_REDIRECT:
                return "Permanent Redirect";

                // 4xx
            case HttpStatus::BAD_REQUEST:
                return "Bad Request";
            case HttpStatus::UNAUTHORIZED:
                return "Unauthorized";
            case HttpStatus::PAYMENT_REQUIRED:
                return "Payment Required";
            case HttpStatus::FORBIDDEN:
                return "Forbidden";
            case HttpStatus::NOT_FOUND:
                return "Not Found";
            case HttpStatus::METHOD_NOT_ALLOWED:
                return "Method Not Allowed";
            case HttpStatus::NOT_ACCEPTABLE:
                return "Not Acceptable";
            case HttpStatus::PROXY_AUTHENTICATION_REQUIRED:
                return "Proxy Authentication Required";
            case HttpStatus::REQUEST_TIMEOUT:
                return "Request Timeout";
            case HttpStatus::CONFLICT:
                return "Conflict";
            case HttpStatus::GONE:
                return "Gone";
            case HttpStatus::LENGTH_REQUIRED:
                return "Length Required";
            case HttpStatus::PRECONDITION_FAILED:
                return "Precondition Failed";
            case HttpStatus::PAYLOAD_TOO_LARGE:
                return "Payload Too Large";
            case HttpStatus::URI_TOO_LONG:
                return "URI Too Long";
            case HttpStatus::UNSUPPORTED_MEDIA_TYPE:
                return "Unsupported Media Type";
            case HttpStatus::RANGE_NOT_SATISFIABLE:
                return "Range Not Satisfiable";
            case HttpStatus::EXPECTATION_FAILED:
                return "Expectation Failed";
            case HttpStatus::I_AM_A_TEAPOT:
                return "I'm a teapot";
            case HttpStatus::UNPROCESSABLE_ENTITY:
                return "Unprocessable Entity";
            case HttpStatus::TOO_EARLY:
                return "Too Early";
            case HttpStatus::UPGRADE_REQUIRED:
                return "Upgrade Required";
            case HttpStatus::PRECONDITION_REQUIRED:
                return "Precondition Required";
            case HttpStatus::TOO_MANY_REQUESTS:
                return "Too Many Requests";
            case HttpStatus::REQUEST_HEADER_FIELDS_TOO_LARGE:
                return "Request Header Fields Too Large";
            case HttpStatus::UNAVAILABLE_FOR_LEGAL_REASONS:
                return "Unavailable For Legal Reasons";

                // 5xx
            case HttpStatus::INTERNAL_SERVER_ERROR:
                return "Internal Server Error";
            case HttpStatus::NOT_IMPLEMENTED:
                return "Not Implemented";
            case HttpStatus::BAD_GATEWAY:
                return "Bad Gateway";
            case HttpStatus::SERVICE_UNAVAILABLE:
                return "Service Unavailable";
            case HttpStatus::GATEWAY_TIMEOUT:
                return "Gateway Timeout";
            case HttpStatus::HTTP_VERSION_NOT_SUPPORTED:
                return "HTTP Version Not Supported";
            case HttpStatus::VARIANT_ALSO_NEGOTIATES:
                return "Variant Also Negotiates";
            case HttpStatus::INSUFFICIENT_STORAGE:
                return "Insufficient Storage";
            case HttpStatus::LOOP_DETECTED:
                return "Loop Detected";
            case HttpStatus::NOT_EXTENDED:
                return "Not Extended";
            case HttpStatus::NETWORK_AUTHENTICATION_REQUIRED:
                return "Network Authentication Required";
            default:
                return "";
        }
    }
} // namespace nets