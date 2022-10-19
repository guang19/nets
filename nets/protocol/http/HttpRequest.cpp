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

#include "nets/protocol/http/HttpRequest.h"

namespace nets
{
    HttpRequest::HttpRequest()
        : method_(HttpMethod::UNKNOWN), url_(), version_(HttpProtocolVersion::UNSUPPORTED), httpHeader_(), requestBody_()
    {
    }

    void HttpRequest::setMethod(HttpMethod method)
    {
        method_ = method;
    }

    HttpMethod HttpRequest::getMethod() const
    {
        return method_;
    }

    void HttpRequest::setUrl(const StringType& url)
    {
        url_ = url;
    }

    const StringType& HttpRequest::getUrl() const
    {
        return url_;
    }

    void HttpRequest::setProtocolVersion(HttpProtocolVersion version)
    {
        version_ = version;
    }

    HttpProtocolVersion HttpRequest::getProtocolVersion() const
    {
        return version_;
    }

    void HttpRequest::setHttpHeader(const StringType& headerName, const StringType& value)
    {
        httpHeader_.insert_or_assign(headerName, value);
    }

    StringType HttpRequest::getHttpHeader(const StringType& headerName) const
    {
        auto it = httpHeader_.find(headerName);
        if (it != httpHeader_.end())
        {
            return it->second;
        }
        else
        {
            return {};
        }
    }

    void HttpRequest::setRequestBody(const StringType& requestBody)
    {
        requestBody_ = requestBody;
    }

    const StringType& HttpRequest::getRequestBody() const
    {
        return requestBody_;
    }
} // namespace nets