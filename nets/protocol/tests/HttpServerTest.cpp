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

// @brief Test for http server

#include <gtest/gtest.h>

#include <memory>

#include "nets/net/bootstrap/ServerBootstrap.h"
#include "nets/protocol/http/HttpDispatcher.h"

using namespace nets;

TEST(HttpServerTest, HttpServer)
{
    //     POST /httptest HTTP/1.1
    //     Content-Length: 0
    //     Host: 192.168.24.131:8080
    //     Connection: Keep-Alive
    //     User-Agent: Apache-HttpClient/4.5.13 (Java/11.0.15)
    //     Accept-Encoding: gzip,def
    ServerBootstrap(8)
        .option(SockOption::SNDBUF, 1024)
        .option(SockOption::RCVBUF, 1024)
        .childHandler(
            [](SocketChannel& channel)
            {
                auto httpDispathcer = ::std::make_shared<HttpDispatcher>("HttpDispatcher");
                channel.pipeline().addLast(httpDispathcer);
            })
        .bind(8080)
        .sync();
}