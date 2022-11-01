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

// @brief Http server codec

#include "nets/protocol/http/HttpServerCodec.h"

#include <stdexcept>

#include "nets/base/StackBuffer.h"
#include "nets/base/StringUtils.h"

namespace nets
{
    namespace
    {
        constexpr char kLF = '\n';
        constexpr char kCRLF[] = "\r\n";
        constexpr char kDoubleCRLF[] = "\r\n\r\n";
        constexpr char kColonSpace[] = ": ";
        constexpr char kColon = ':';
        constexpr char kSpace = ' ';
        constexpr char kQuestion = '?';
        constexpr char kAnd = '&';
        constexpr char kEqual = '=';
    } // namespace

    bool HttpServerCodec::decode(const StringType& message, HttpRequest& httpRequest)
    {
        if (message.length() == 0)
        {
            return false;
        }
        SizeType requestLineEnd = message.find(kCRLF);
        if (requestLineEnd == StringType::npos)
        {
            return false;
        }
        if (!parseRequestLine(message.substr(0, requestLineEnd), httpRequest))
        {
            return false;
        }
        SizeType requestHeaderEnd = message.find(kDoubleCRLF, requestLineEnd);
        if (requestHeaderEnd == StringType::npos)
        {
            return false;
        }
        if (!parseRequestHeader(message.substr(requestLineEnd + 2, requestHeaderEnd - requestLineEnd), httpRequest))
        {
            return false;
        }
        if (!parseRequestBody(message, requestHeaderEnd + 4, httpRequest))
        {
            return false;
        }
        return true;
    }

    bool HttpServerCodec::encode(StringType& message, const HttpResponse& httpResponse)
    {
        message.append(httpProtocolVersionToString(httpResponse.getProtocolVersion()));
        message += kSpace;
        StackBuffer<kMaximumNumberLimit + 1> statusCodeBuf {};
        statusCodeBuf.writeInt32(httpStatusToCode(httpResponse.getStatus()));
        message.append(statusCodeBuf.array(), statusCodeBuf.length());
        message += kSpace;
        message.append(httpStatusToText(httpResponse.getStatus()));
        message.append(kCRLF, 2);
        const auto& httpHeaders = httpResponse.getHeaders();
        for (const auto& httpHeader : httpHeaders)
        {
            message.append(httpHeader.first);
            message += kColon;
            message.append(httpHeader.second);
            message.append(kCRLF, 2);
        }
        message.append(kCRLF, 2);
        message.append(httpResponse.getBody());
        return true;
    }

    bool HttpServerCodec::parseRequestLine(const StringType& requestLine, HttpRequest& httpRequest)
    {
        ::std::vector<StringType> parts {};
        utils::split(requestLine, parts, ' ');
        if (parts.size() != 3)
        {
            return false;
        }
        httpRequest.setMethod(stringToHttpMethod(parts[0]));
        httpRequest.setUrl(parts[1]);
        parseRequestQueryParameters(parts[1], httpRequest);
        httpRequest.setProtocolVersion(stringToHttpProtocolVersion(parts[2]));
        return true;
    }

    void HttpServerCodec::parseRequestQueryParameters(const StringType& url, HttpRequest& httpRequest)
    {
        SizeType question = url.find(kQuestion);
        if (StringType::npos != question && question < url.length() - 1)
        {
            StringType queryStr = url.substr(question + 1);
            ::std::vector<StringType> queryParameterStrs {};
            utils::split(queryStr, queryParameterStrs, kAnd);
            SizeType equal = 0;
            for (const auto& queryParameterStr : queryParameterStrs)
            {
                equal = queryParameterStr.find(kEqual);
                if (StringType::npos != equal)
                {
                    httpRequest.setQueryParameter(queryParameterStr.substr(0, equal), queryParameterStr.substr(equal + 1));
                }
            }
        }
    }

    bool HttpServerCodec::parseRequestHeader(const StringType& requestHeader, HttpRequest& httpRequest)
    {
        ::std::vector<StringType> headerLines {};
        utils::split(requestHeader, headerLines, kCRLF);
        SizeType colonAfterHeaderName = 0;
        for (const auto& headerLine : headerLines)
        {
            colonAfterHeaderName = headerLine.find(kColonSpace);
            if (StringType::npos == colonAfterHeaderName)
            {
                return false;
            }
            httpRequest.setHeader(headerLine.substr(0, colonAfterHeaderName), headerLine.substr(colonAfterHeaderName + 2));
        }
        return true;
    }

    bool HttpServerCodec::parseRequestBody(const StringType& data, SizeType requestBodyStart, HttpRequest& httpRequest)
    {
        StringType contentLengthHttpHeaderName = httpHeaderToString(HttpHeader::CONTENT_LENGTH);
        // has no request body
        if (!httpRequest.hasHeader(contentLengthHttpHeaderName))
        {
            return true;
        }
        StringType contentLengthStr = httpRequest.getHeader(contentLengthHttpHeaderName);
        SizeType contentLength = 0;
        try
        {
            contentLength = ::std::stol(contentLengthStr);
            if (contentLengthStr.length() != ::std::to_string(contentLength).length())
            {
                return false;
            }
        }
        catch (const ::std::invalid_argument& e)
        {
            return false;
        }
        if (contentLength == 0)
        {
            return true;
        }
        if (contentLength != data.length() - requestBodyStart)
        {
            return false;
        }
        httpRequest.setBody(data.substr(requestBodyStart, contentLength));
        return true;
    }
} // namespace nets