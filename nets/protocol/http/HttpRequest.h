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

// @brief http request

#ifndef NETS_HTTP_REQUEST_H
#define NETS_HTTP_REQUEST_H

#include "nets/base/Noncopyable.h"
#include "nets/protocol/http/HttpCommon.h"

namespace nets
{
    class HttpRequest : Noncopyable
    {
    public:
        HttpRequest();
        ~HttpRequest() = default;

    public:
        void setMethod(HttpMethod method);
        HttpMethod getMethod() const;

        void setUrl(const StringType& url);
        const StringType& getUrl() const;

        void setProtocolVersion(HttpProtocolVersion version);
        HttpProtocolVersion getProtocolVersion() const;

        void setHttpHeader(const StringType& headerName, const StringType& value);
        StringType getHttpHeader(const StringType& headerName) const;

        void setRequestBody(const StringType& requestBody);
        const StringType& getRequestBody() const;

    private:
        HttpMethod method_;
        StringType url_;
        HttpProtocolVersion version_;
        HttpHeaderContainer httpHeader_;
        StringType requestBody_;
    };
} // namespace nets

#endif // NETS_HTTP_REQUEST_H