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

// @brief http codec

#include "nets/protocol/http/HttpCodec.h"

namespace nets
{
    namespace
    {
        enum class State
        {
            PARSE_REQUEST_LINE,
            PARSE_REQUEST_HEADER,
            PARSE_REQUEST_BODY
        };
        constexpr char kLF = '\n';
        constexpr char kCRLF[] = "\r\n";
        constexpr char kSpace = ' ';
    } // namespace

    bool HttpCodec::decode(const StringType& data, HttpRequest& httpRequest)
    {
        StringType temp(data);
        State currentState = State::PARSE_REQUEST_LINE;
        switch (currentState)
        {
            case State::PARSE_REQUEST_LINE:
            {
                SizeType requestLineCRLFIndex = temp.find_first_of(kCRLF);
                if (requestLineCRLFIndex == StringType::npos)
                {
                    return false;
                }
                if (parseRequestLine(temp.substr(0, requestLineCRLFIndex), httpRequest))
                {
                    currentState = State::PARSE_REQUEST_HEADER;
                }
                else
                {
                    return false;
                }
            }
            case State::PARSE_REQUEST_HEADER:
            {
            }
            case State::PARSE_REQUEST_BODY:
            {
            }
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
        HttpMethod method = stringToMethod(requestLine.substr(0, methodEnd));
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
        HttpProtocolVersion protocolVersion = stringToProtocolVersion(requestLine.substr(urlEnd + 1, requestLine.length() - urlEnd - 1));
        if (protocolVersion == HttpProtocolVersion::UNSUPPORTED)
        {
            return false;
        }
        httpRequest.setProtocolVersion(protocolVersion);
        return true;
    }
} // namespace nets