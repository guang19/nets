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

// @brief http response

#include "nets/protocol/http/HttpResponse.h"

namespace nets
{
    HttpResponse::HttpResponse()
        : version_(HttpProtocolVersion::UNSUPPORTED), statusCode_(HttpStatusCode::UNKNOWN), statusText_(), httpHeader_(),
          responseBody_()
    {
    }

    void HttpResponse::setProtocolVersion(HttpProtocolVersion version)
    {
        version_ = version;
    }

    HttpProtocolVersion HttpResponse::getProtocolVersion() const
    {
        return version_;
    }

    void HttpResponse::setStatusCode(HttpStatusCode statusCode)
    {
        statusCode_ = statusCode;
    }

    HttpStatusCode HttpResponse::getStatusCode() const
    {
        return statusCode_;
    }

    void HttpResponse::setStatusText(const StringType& statusText)
    {
        statusText_ = statusText;
    }

    const StringType& HttpResponse::getStatusText() const
    {
        return statusText_;
    }

    void HttpResponse::setHttpHeader(const StringType& headerName, const StringType& value)
    {
        httpHeader_.insert_or_assign(headerName, value);
    }

    StringType HttpResponse::getHttpHeader(const StringType& headerName) const
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

    void HttpResponse::setResponseBody(const StringType& responseBody)
    {
        responseBody_ = responseBody;
    }

    const StringType& HttpResponse::getResponseBody() const
    {
        return responseBody_;
    }
} // namespace nets