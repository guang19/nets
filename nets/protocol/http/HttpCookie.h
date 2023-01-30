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

// @brief Http cookie

#ifndef NETS_HTTP_COOKIE_H
#define NETS_HTTP_COOKIE_H

#include "nets/base/Copyable.h"
#include "nets/base/Types.h"

namespace nets
{
    enum class SameSite
    {
        STRICT,
        LAX,
        NONE
    };

    class HttpCookie : public Copyable
    {
    public:
        static const HttpCookie kInvalidCookie;

    public:
        HttpCookie();
        ~HttpCookie() = default;

        HttpCookie(const HttpCookie& other);
        HttpCookie(HttpCookie&& other) noexcept;
        HttpCookie& operator=(const HttpCookie& other);
        HttpCookie& operator=(HttpCookie&& other) noexcept;

    public:
        inline const StringType& getName() const
        {
            return name_;
        }

        inline void setName(const StringType& name)
        {
            name_ = name;
        }

        inline const StringType& getValue() const
        {
            return value_;
        }

        inline void setValue(const StringType& value)
        {
            value_ = value;
        }

        inline const StringType& getDomain() const
        {
            return domain_;
        }

        inline void setDomain(const StringType& domain)
        {
            domain_ = domain;
        }

        inline const StringType& getPath() const
        {
            return path_;
        }

        inline void setPath(const StringType& path)
        {
            path_ = path;
        }

        inline const StringType& getExpires() const
        {
            return expires_;
        }

        inline void setExpires(const StringType& expires)
        {
            expires_ = expires;
        }

        inline Int32Type getMaxAge() const
        {
            return maxAge_;
        }

        inline void setMaxAge(Int32Type maxAge)
        {
            maxAge_ = maxAge;
        }

        inline bool getHttpOnly() const
        {
            return httpOnly_;
        }

        inline void setHttpOnly(bool httpOnly)
        {
            httpOnly_ = httpOnly;
        }

        inline bool getSecure() const
        {
            return secure_;
        }

        inline void setSecure(bool secure)
        {
            secure_ = secure;
        }

        inline SameSite getSameSite() const
        {
            return sameSite_;
        }

        inline void setSameSite(SameSite sameSite)
        {
            sameSite_ = sameSite;
        }

    private:
        StringType name_;
        StringType value_;
        StringType domain_;
        StringType path_;
        StringType expires_;
        Int32Type maxAge_;
        bool httpOnly_;
        bool secure_;
        SameSite sameSite_;
    };
} // namespace nets

#endif // NETS_HTTP_COOKIE_H