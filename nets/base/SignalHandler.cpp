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

#include "nets/base/SignalHandler.h"

#include "nets/base/Common.h"

namespace nets
{
    SignalHandler::SignalHandlerType SignalHandler::signalHandler_ = nullptr;

    void SignalHandler::initSignalHandler(const SignalHandlerType& signalHandler)
    {
        signalHandler_ = signalHandler;
        Sigaction act {};
        MEMZERO(&act, sizeof(Sigaction));
        act.sa_sigaction = SignalHandler::handleSignal;
        act.sa_flags = SA_SIGINFO | SA_RESTART;
        sigaction(SIGPIPE, &act, nullptr);
        sigaction(SIGHUP, &act, nullptr);
        sigaction(SIGINT, &act, nullptr);
        sigaction(SIGQUIT, &act, nullptr);
        sigaction(SIGTERM, &act, nullptr);
        sigaction(SIGSEGV, &act, nullptr);
    }
} // namespace nets