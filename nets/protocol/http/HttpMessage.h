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

#ifndef NETS_HTTP_MESSAGE_H
#define NETS_HTTP_MESSAGE_H

#include <map>

#include "nets/base/Noncopyable.h"
#include "nets/protocol/http/HttpCommon.h"
#include "nets/protocol/http/HttpCookie.h"

namespace nets
{
    class HttpMessage : Noncopyable
    {
    private:
        using HttpHeaderContainer = ::std::map<StringType, StringType>;

    public:
        HttpMessage();
        ~HttpMessage() = default;

        inline void setProtocolVersion(HttpProtocolVersion version)
        {
            version_ = version;
        }

        HttpProtocolVersion getProtocolVersion() const
        {
            return version_;
        }

        void setHeader(const StringType& name, const StringType& value);
        const StringType& getHeader(const StringType& name, const StringType& defaultValue = "") const;
        bool hasHeader(const StringType& name) const;

        inline const HttpHeaderContainer& getHeaders() const
        {
            return headers_;
        }

        void setBody(const StringType& body)
        {
            body_ = body;
        }

        const StringType& getBody() const
        {
            return body_;
        }

    protected:
        HttpProtocolVersion version_;
        HttpHeaderContainer headers_;
        StringType body_;
    };
} // namespace nets

#endif // NETS_HTTP_MESSAGE_H