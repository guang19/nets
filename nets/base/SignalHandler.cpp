//
// Created by guang19
//

#include "nets/base/SignalHandler.h"

#include "nets/base/CommonMacro.h"

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