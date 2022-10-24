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

// @brief Http codec

#include "nets/protocol/http/HttpCodec.h"

#include <stdexcept>

namespace nets
{
    namespace
    {
        constexpr char kLF = '\n';
        constexpr char kCRLF[] = "\r\n";
        constexpr char kTwoCRLF[] = "\r\n\r\n";
        constexpr char kColonSpace[] = ": ";
        constexpr char kSpace = ' ';
    } // namespace

    bool HttpCodec::decode(const StringType& data, HttpRequest& httpRequest)
    {
        SizeType requestLineEnd = data.find(kCRLF);
        if (requestLineEnd == StringType::npos)
        {
            return false;
        }
        if (!parseRequestLine(data.substr(0, requestLineEnd), httpRequest))
        {
            return false;
        }
        SizeType requestHeaderEnd = data.find(kTwoCRLF, requestLineEnd);
        if (requestHeaderEnd == StringType::npos)
        {
            return false;
        }
        if (!parseRequestHeader(data.substr(requestLineEnd + 2, requestHeaderEnd - requestLineEnd - 2), httpRequest))
        {
            return false;
        }
        if (!parseRequestBody(data, requestHeaderEnd + 4, httpRequest))
        {
            return false;
        }
        return true;
    }

    bool HttpCodec::parseRequestLine(const StringType& requestLine, HttpRequest& httpRequest)
    {
        SizeType methodEnd = requestLine.find_first_of(kSpace);
        if (methodEnd == StringType::npos)
        {
            return false;
        }
        HttpMethod method = stringToHttpMethod(requestLine.substr(0, methodEnd));
        if (method == HttpMethod::UNKNOWN)
        {
            return false;
        }
        httpRequest.setMethod(method);
        SizeType urlEnd = requestLine.find_last_of(kSpace);
        if (urlEnd == StringType::npos || urlEnd == methodEnd)
        {
            return false;
        }
        HttpProtocolVersion protocolVersion =
            stringToHttpProtocolVersion(requestLine.substr(urlEnd + 1, requestLine.length() - urlEnd - 1));
        if (protocolVersion == HttpProtocolVersion::UNSUPPORTED)
        {
            return false;
        }
        httpRequest.setProtocolVersion(protocolVersion);
        StringType url = requestLine.substr(methodEnd + 1, urlEnd - methodEnd - 1);
        // TODO:parse url query parameter
        httpRequest.setUrl(url);
        return true;
    }

    bool HttpCodec::parseRequestHeader(const StringType& requestHeader, HttpRequest& httpRequest)
    {
        bool isLastLine = false;
        StringType headerLine {}, headerName {}, headerValue {};
        SizeType colonAfterHeaderName = 0;
        SizeType lastPos = 0, pos = requestHeader.find(kCRLF, lastPos);
        for (;;)
        {
            if (pos == StringType::npos)
            {
                isLastLine = true;
                // has no "\r\n" on the requestHeader last line
                headerLine = requestHeader.substr(lastPos);
            }
            else
            {
                headerLine = requestHeader.substr(lastPos, pos - lastPos);
            }
            colonAfterHeaderName = headerLine.find(kColonSpace);
            if (colonAfterHeaderName == StringType::npos)
            {
                return false;
            }
            httpRequest.setHttpHeader(headerLine.substr(0, colonAfterHeaderName),
                                      headerLine.substr(colonAfterHeaderName + 2));
            if (isLastLine)
            {
                break;
            }
            lastPos = pos + 2;
            pos = requestHeader.find(kCRLF, lastPos);
        }
        return true;
    }

    bool HttpCodec::parseRequestBody(const StringType& data, SizeType requestBodyStart, HttpRequest& httpRequest)
    {
        StringType contentLengthHttpHeaderName = httpHeaderToString(HttpHeader::CONTENT_LENGTH);
        // has no request body
        if (!httpRequest.hasHttpHeader(contentLengthHttpHeaderName))
        {
            return true;
        }
        StringType contentLengthStr = httpRequest.getHttpHeader(contentLengthHttpHeaderName);
        SizeType contentLength = 0;
        try
        {
            contentLength = ::std::stol(contentLengthStr);
            if (contentLength == 0 && contentLengthStr.length() != ::std::to_string(contentLength).length())
            {
                return false;
            }
        }
        catch (const ::std::invalid_argument& e)
        {
            return false;
        }
        if (contentLength != data.length() - requestBodyStart)
        {
            return false;
        }
        httpRequest.setRequestBody(data.substr(requestBodyStart, contentLength));
        return true;
    }
} // namespace nets