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

// @brief Http request dispatcher

#include "nets/protocol/http/HttpDispatcher.h"

#include "nets/base/log/Logger.h"
#include "nets/net/core/ByteBuffer.h"
#include "nets/net/core/SocketChannelContext.h"

namespace nets
{
    HttpDispatcher::HttpDispatcher() : serverCodec_() {}
    HttpDispatcher::HttpDispatcher(const StringType& name) : SocketChannelHandler(name), serverCodec_() {}

    void HttpDispatcher::channelRead(SocketChannelContext& channelContext, ByteBuffer& message)
    {
        StringType reqMsg(message.toString());
        NETS_SYSTEM_LOG_DEBUG << reqMsg;
        HttpRequest httpRequest {};
        if (serverCodec_.decode(reqMsg, httpRequest))
        {
            HttpResponse httpResponse {};
            httpResponse.setProtocolVersion(httpRequest.getProtocolVersion());
            dispatch(httpRequest, httpResponse);
            StringType repMsg{};
            serverCodec_.encode(repMsg, httpResponse);
            NETS_SYSTEM_LOG_DEBUG << repMsg;
            channelContext.write(repMsg);
        }
        else
        {
            channelContext.disconnect();
        }
    }

    void HttpDispatcher::dispatch(HttpRequest& httpRequest, HttpResponse& httpResponse)
    {
        if (httpRequest.getProtocolVersion() == HttpProtocolVersion::UNSUPPORTED)
        {
            httpResponse.setStatus(HttpStatus::HTTP_VERSION_NOT_SUPPORTED);
        }
    }
} // namespace nets