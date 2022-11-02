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

// @brief Http request

#include "nets/protocol/http/HttpRequest.h"

namespace nets
{
    HttpRequest::HttpRequest() : method_(HttpMethod::UNSUPPORTED), url_(), queryParameters_(), cookies_() {}

    void HttpRequest::setQueryParameter(const StringType& name, const StringType& value)
    {
        queryParameters_.insert_or_assign(name, value);
    }

    const StringType& HttpRequest::getQueryParameter(const StringType& name, const StringType& defaultValue)
    {
        auto it = queryParameters_.find(name);
        return it != queryParameters_.end() ? it->second : defaultValue;
    }

    bool HttpRequest::hasQueryParameter(const StringType& name) const
    {
        return queryParameters_.find(name) != queryParameters_.end();
    }

    void HttpRequest::addCookie(const StringType& name, const StringType& value)
    {
        cookies_.insert_or_assign(name, value);
    }

    const StringType& HttpRequest::getCookie(const StringType& name, const StringType& defaultValue)
    {
        auto it = cookies_.find(name);
        return it != cookies_.end() ? it->second : defaultValue;
    }
} // namespace nets