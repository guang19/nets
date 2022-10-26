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

#include "nets/protocol/http/HttpCookie.h"

namespace nets
{
    const HttpCookie HttpCookie::kInvalidCookie {};

    HttpCookie::HttpCookie()
        : name_(), value_(), domain_(), path_(), expires_(), maxAge_(0), httpOnly_(false), secure_(false),
          sameSite_(SameSite::LAX)
    {
    }

    HttpCookie::HttpCookie(const HttpCookie& other)
        : name_(other.name_), value_(other.value_), domain_(other.domain_), path_(other.path_), expires_(other.expires_),
          maxAge_(other.maxAge_), httpOnly_(other.httpOnly_), secure_(other.secure_), sameSite_(other.sameSite_)
    {
    }

    HttpCookie::HttpCookie(HttpCookie&& other) noexcept
        : name_(::std::move(other.name_)), value_(::std::move(other.value_)), domain_(::std::move(other.domain_)),
          path_(::std::move(other.path_)), expires_(::std::move(other.expires_)), maxAge_(other.maxAge_),
          httpOnly_(other.httpOnly_), secure_(other.secure_), sameSite_(other.sameSite_)
    {
        other.maxAge_ = 0;
        other.httpOnly_ = false;
        other.secure_ = false;
        other.sameSite_ = SameSite::LAX;
    }

    HttpCookie& HttpCookie::operator=(const HttpCookie& other)
    {
        if (this != &other)
        {
            name_ = other.name_;
            value_ = other.value_;
            domain_ = other.domain_;
            path_ = other.path_;
            expires_ = other.expires_;
            maxAge_ = other.maxAge_;
            httpOnly_ = other.httpOnly_;
            secure_ = other.secure_;
            sameSite_ = other.sameSite_;
        }
        return *this;
    }

    HttpCookie& HttpCookie::operator=(HttpCookie&& other) noexcept
    {
        if (this != &other)
        {
            name_ = ::std::move(other.name_);
            value_ = ::std::move(other.value_);
            domain_ = ::std::move(other.domain_);
            path_ = ::std::move(other.path_);
            expires_ = ::std::move(other.expires_);
            maxAge_ = other.maxAge_;
            httpOnly_ = other.httpOnly_;
            secure_ = other.secure_;
            sameSite_ = other.sameSite_;
            other.maxAge_ = 0;
            other.httpOnly_ = false;
            other.secure_ = false;
            other.sameSite_ = SameSite::LAX;
        }
        return *this;
    }
} // namespace nets