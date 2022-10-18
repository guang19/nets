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

// @brief Kernel signal handler

#ifndef NETS_SIGNAL_HANDLER_H
#define NETS_SIGNAL_HANDLER_H

#include <csignal>
#include <functional>

#include "nets/base/Noncopyable.h"
#include "nets/base/Types.h"

namespace nets
{
    class SignalHandler : Noncopyable
    {
    public:
        using SignoType = Int32Type;
        using Sigaction = struct sigaction;
        using SigInfo = siginfo_t;
        using SignalHandlerType = ::std::function<void(SignoType signo, SigInfo* info, void* context)>;

    public:
        explicit SignalHandler() = default;
        ~SignalHandler() = default;

    public:
        static void initSignalHandler(const SignalHandlerType& signalHandler);

        static void handleSignal(SignoType signo, SigInfo* info, void* context)
        {
            if (signalHandler_ != nullptr)
            {
                signalHandler_(signo, info, context);
            }
        }

    private:
        static SignalHandlerType signalHandler_;
    };
} // namespace nets

#endif // NETS_SIGNAL_HANDLER_H