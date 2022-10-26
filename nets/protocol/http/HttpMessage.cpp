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

// @brief Http common message

#include "nets/protocol/http/HttpMessage.h"

namespace nets
{
    HttpMessage::HttpMessage() : version_(HttpProtocolVersion::UNSUPPORTED), headers_(), cookies_(), body_() {}

    void HttpMessage::setHeader(const StringType& name, const StringType& value)
    {
        headers_.insert_or_assign(name, value);
    }

    const StringType& HttpMessage::getHeader(const StringType& name, const StringType& defaultValue) const
    {
        auto it = headers_.find(name);
        return it != headers_.end() ? it->second : defaultValue;
    }

    bool HttpMessage::hasHeader(const StringType& name) const
    {
        return headers_.find(name) != headers_.end();
    }

    void HttpMessage::addCookie(const HttpCookie& cookie)
    {
        cookies_.insert_or_assign(cookie.getName(), cookie);
    }

    const HttpCookie& HttpMessage::getCookie(const StringType& name)
    {
        auto it = cookies_.find(name);
        return it != cookies_.end() ? it->second : HttpCookie::kInvalidCookie;
    }
} // namespace nets