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

#include "nets/base/log/Logger.h"
#include "nets/base/StackBuffer.h"
#include "nets/base/StringUtils.h"

namespace nets
{
    namespace
    {
        constexpr char kLF = '\n';
        constexpr char kCRLF[] = "\r\n";
        constexpr char kDoubleCRLF[] = "\r\n\r\n";
        constexpr char kSemicolonSpace[] = "; ";
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
        parseRequestHeader(message.substr(requestLineEnd + 2, requestHeaderEnd - requestLineEnd), httpRequest);
        parseRequestBody(message, requestHeaderEnd + 4, httpRequest);
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
        if (StringType::npos != question && question < url.length() - 3)
        {
            StringType queryStr = url.substr(question + 1);
            ::std::vector<StringType> queryParameterStrs {};
            utils::split(queryStr, queryParameterStrs, kAnd);
            SizeType equal = 0;
            for (const auto& queryParameterStr : queryParameterStrs)
            {
                equal = queryParameterStr.find(kEqual);
                if (equal < queryParameterStr.length() - 1)
                {
                    if (StringType::npos != equal)
                    {
                        httpRequest.setQueryParameter(queryParameterStr.substr(0, equal),
                                                      queryParameterStr.substr(equal + 1));
                    }
                }
            }
        }
    }

    void HttpServerCodec::parseRequestHeader(const StringType& requestHeader, HttpRequest& httpRequest)
    {
        ::std::vector<StringType> headerLines {};
        utils::split(requestHeader, headerLines, kCRLF);
        SizeType colon = 0;
        for (const auto& headerLine : headerLines)
        {
            colon = headerLine.find(kColon);
            if (StringType::npos != colon && colon < headerLine.length() - 1)
            {
                StringType value = headerLine.substr(colon + 1);
                utils::trimL(value);
                httpRequest.setHeader(headerLine.substr(0, colon), value);
            }
        }
        parseRequestCookie(httpRequest);
    }

    void HttpServerCodec::parseRequestCookie(HttpRequest& httpRequest)
    {
        StringType cookieHttpHeaderName = httpHeaderToString(HttpHeader::COOKIE);
        if (httpRequest.hasHeader(cookieHttpHeaderName))
        {
            StringType cookieStr = httpRequest.getHeader(cookieHttpHeaderName);
            ::std::vector<StringType> cookieLines {};
            utils::split(cookieStr, cookieLines, kSemicolonSpace);
            SizeType equal = 0;
            for (const auto& cookieLine : cookieLines)
            {
                equal = cookieLine.find(kEqual);
                if (equal < cookieLine.length() - 1)
                {
                    if (StringType::npos != equal)
                    {
                        httpRequest.addCookie(cookieLine.substr(0, equal), cookieLine.substr(equal + 1));
                    }
                }
            }
        }
    }

    void HttpServerCodec::parseRequestBody(const StringType& data, SizeType requestBodyStart, HttpRequest& httpRequest)
    {
        StringType contentLengthHttpHeaderName = httpHeaderToString(HttpHeader::CONTENT_LENGTH);
        // has no request body
        if (httpRequest.hasHeader(contentLengthHttpHeaderName))
        {
            StringType contentLengthStr = httpRequest.getHeader(contentLengthHttpHeaderName);
            SizeType contentLength = 0, pos = 0;
            try
            {
                contentLength = ::std::stol(contentLengthStr, &pos);
            }
            catch (const ::std::invalid_argument& e)
            {
                NETS_SYSTEM_LOG_WARN << "HttpServerCodec parse http request body,Content-Length is invalid";
            }
            if (contentLengthStr.length() != pos)
            {
                httpRequest.setBody(data.substr(requestBodyStart));
            }
            else
            {
                httpRequest.setBody(data.substr(requestBodyStart, contentLength));
            }
        }
    }
} // namespace nets