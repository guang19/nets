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

#ifndef NETS_HTTP_COMMON_H
#define NETS_HTTP_COMMON_H

#include "nets/base/Types.h"

namespace nets
{
    enum class HttpMethod
    {
        CONNECT = 0,
        DELETE,
        GET,
        HEAD,
        OPTIONS,
        PATCH,
        POST,
        PUT,
        TRACE,
        UNKNOWN
    };

    StringType httpMethodToString(HttpMethod method);
    HttpMethod stringToHttpMethod(const StringType& methodStr);

    enum class HttpProtocolVersion
    {
        HTTP_1_0 = 0,
        HTTP_1_1,
        UNSUPPORTED,
    };

    StringType httpProtocolVersionToString(HttpProtocolVersion protocolVersion);
    HttpProtocolVersion stringToHttpProtocolVersion(const StringType& protocolVersionStr);

    enum class HttpHeader
    {
        ACCEPT = 0,
        ACCEPT_CH,
        ACCEPT_CH_LIFETIME,
        ACCEPT_CHARSET,
        ACCEPT_ENCODING,
        ACCEPT_LANGUAGE,
        ACCEPT_PATCH,
        ACCEPT_POST,
        ACCEPT_RANGES,
        ACCESS_CONTROL_ALLOW_CREDENTIALS,
        ACCESS_CONTROL_ALLOW_HEADERS,
        ACCESS_CONTROL_ALLOW_METHODS,
        ACCESS_CONTROL_ALLOW_ORIGIN,
        ACCESS_CONTROL_EXPOSE_HEADERS,
        ACCESS_CONTROL_MAX_AGE,
        ACCESS_CONTROL_REQUEST_HEADERS,
        ACCESS_CONTROL_REQUEST_METHOD,
        AGE,
        ALLOW,
        ALT_SVC,
        AUTHORIZATION,
        CACHE_CONTROL,
        CLEAR_SITE_DATA,
        CONNECTION,
        CONTENT_DISPOSITION,
        CONTENT_DPR,
        CONTENT_ENCODING,
        CONTENT_LANGUAGE,
        CONTENT_LENGTH,
        CONTENT_LOCATION,
        CONTENT_RANGE,
        CONTENT_SECURITY_POLICY,
        CONTENT_SECURITY_POLICY_REPORT_ONLY,
        CONTENT_TYPE,
        COOKIE,
        CROSS_ORIGIN_EMBEDDER_POLICY,
        CROSS_ORIGIN_OPENER_POLICY,
        CROSS_ORIGIN_RESOURCE_POLICY,
        DATE,
        DEVICE_MEMORY,
        DIGEST,
        DNT,
        DOWNLINK,
        DPR,
        EARLY_DATA,
        ECT,
        ETAG,
        EXPECT,
        EXPECT_CT,
        EXPIRES,
        FEATURE_POLICY,
        FORWARDED,
        FROM,
        HOST,
        IF_MATCH,
        IF_MODIFIED_SINCE,
        IF_NONE_MATCH,
        IF_RANGE,
        IF_UNMODIFIED_SINCE,
        KEEP_ALIVE,
        LARGE_ALLOCATION,
        LAST_MODIFIED,
        LINK,
        LOCATION,
        MAX_FORWARDS,
        NEL,
        ORIGIN,
        PRAGMA,
        PROXY_AUTHENTICATE,
        PROXY_AUTHORIZATION,
        RANGE,
        REFERER,
        REFERRER_POLICY,
        RETRY_AFTER,
        RTT,
        SAVE_DATA,
        SEC_CH_UA,
        SEC_CH_UA_ARCH,
        SEC_CH_UA_BITNESS,
        SEC_CH_UA_FULL_VERSION,
        SEC_CH_UA_FULL_VERSION_LIST,
        SEC_CH_UA_MOBILE,
        SEC_CH_UA_MODEL,
        SEC_CH_UA_PLATFORM,
        SEC_CH_UA_PLATFORM_VERSION,
        SEC_FETCH_DEST,
        SEC_FETCH_MODE,
        SEC_FETCH_SITE,
        SEC_FETCH_USER,
        SEC_GPC,
        SEC_WEBSOCKET_ACCEPT,
        SERVER,
        SERVER_TIMING,
        SERVICE_WORKER_NAVIGATION_PRELOAD,
        SET_COOKIE,
        SOURCEMAP,
        STRICT_TRANSPORT_SECURITY,
        TE,
        TIMING_ALLOW_ORIGIN,
        TK,
        TRAILER,
        TRANSFER_ENCODING,
        UPGRADE,
        UPGRADE_INSECURE_REQUESTS,
        USER_AGENT,
        VARY,
        VIA,
        VIEWPORT_WIDTH,
        WANT_DIGEST,
        WARNING,
        WIDTH,
        WWW_AUTHENTICATE,
        X_CONTENT_TYPE_OPTIONS,
        X_DNS_PREFETCH_CONTROL,
        X_FORWARDED_FOR,
        X_FORWARDED_HOST,
        X_FORWARDED_PROTO,
        X_FRAME_OPTIONS,
        X_XSS_PROTECTION
    };

    StringType httpHeaderToString(HttpHeader httpHeader);

    enum class HttpStatus
    {
        // 1xx
        CONTINUE = 100,
        SWITCHING_PROTOCOLS = 101,
        EARLY_HINTS = 103,
        // 2xx
        OK = 200,
        CREATED = 201,
        ACCEPTED = 202,
        NON_AUTHORITATIVE_INFORMATION = 203,
        NO_CONTENT = 204,
        RESET_CONTENT = 205,
        PARTIAL_CONTENT = 206,
        // 3xx
        MULTIPLE_CHOICES = 300,
        MOVED_PERMANENTLY = 301,
        FOUND = 302,
        SEE_OTHER = 303,
        NOT_MODIFIED = 304,
        TEMPORARY_REDIRECT = 307,
        PERMANENT_REDIRECT = 308,
        // 4xx
        BAD_REQUEST = 400,
        UNAUTHORIZED = 401,
        PAYMENT_REQUIRED = 402,
        FORBIDDEN = 403,
        NOT_FOUND = 404,
        METHOD_NOT_ALLOWED = 405,
        NOT_ACCEPTABLE = 406,
        PROXY_AUTHENTICATION_REQUIRED = 407,
        REQUEST_TIMEOUT = 408,
        CONFLICT = 409,
        GONE = 410,
        LENGTH_REQUIRED = 411,
        PRECONDITION_FAILED = 412,
        PAYLOAD_TOO_LARGE = 413,
        URI_TOO_LONG = 414,
        UNSUPPORTED_MEDIA_TYPE = 415,
        RANGE_NOT_SATISFIABLE = 416,
        EXPECTATION_FAILED = 417,
        I_AM_A_TEAPOT = 418,
        UNPROCESSABLE_ENTITY = 422,
        TOO_EARLY = 425,
        UPGRADE_REQUIRED = 426,
        PRECONDITION_REQUIRED = 428,
        TOO_MANY_REQUESTS = 429,
        REQUEST_HEADER_FIELDS_TOO_LARGE = 431,
        UNAVAILABLE_FOR_LEGAL_REASONS = 451,
        // 5xx
        INTERNAL_SERVER_ERROR = 500,
        NOT_IMPLEMENTED = 501,
        BAD_GATEWAY = 502,
        SERVICE_UNAVAILABLE = 503,
        GATEWAY_TIMEOUT = 504,
        HTTP_VERSION_NOT_SUPPORTED = 505,
        VARIANT_ALSO_NEGOTIATES = 506,
        INSUFFICIENT_STORAGE = 507,
        LOOP_DETECTED = 508,
        NOT_EXTENDED = 510,
        NETWORK_AUTHENTICATION_REQUIRED = 511
    };

    HttpStatus statusCodeToHttpStatus(Int32Type statusCode);
    Int32Type httpStatusToCode(HttpStatus httpStatus);
    StringType httpStatusToText(HttpStatus httpStatus);
} // namespace nets

#endif // NETS_HTTP_COMMON_H