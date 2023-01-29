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

// @brief Log formatter

#ifndef NETS_LOG_FORMATTER_H
#define NETS_LOG_FORMATTER_H

#include "nets/base/log/LogBuffer.h"
#include "nets/base/Singleton.h"
#include "nets/base/Timestamp.h"

namespace nets
{
    enum class LogLevel;
    class LogMessage;
    class LogFormatter
    {
    protected:
        LogFormatter() = default;
        virtual ~LogFormatter() = default;

    public:
        virtual void formatLogMessage(LogLevel logLevel, const LogMessage& logMessage, LogBuffer& logBuffer) = 0;
    };

    DECLARE_SINGLETON_CLASS(DefaultLogFormatter), public LogFormatter
    {
        DEFINE_SINGLETON(DefaultLogFormatter);

    private:
        DefaultLogFormatter() = default;
        ~DefaultLogFormatter() override = default;

    public:
        void formatLogMessage(LogLevel logLevel, const LogMessage& logMessage, LogBuffer& logBuffer) override;

    private:
        void formatLogTime(const Timestamp& logTime, LogBuffer& logBuffer);
    };
} // namespace nets

#endif // NETS_LOG_FORMATTER_H