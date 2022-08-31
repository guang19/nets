//
// Created by guang19
//

#include "nets/base/SignalHandler.h"

#include "nets/base/CommonMacro.h"

namespace nets::base
{
    SignalHandler::SignalHandlerType SignalHandler::_SignalHandler = nullptr;

    void SignalHandler::initSignalHandler(const SignalHandlerType& signalHandler)
    {
        _SignalHandler = signalHandler;
        Sigaction act {};
        MEMZERO(&act, sizeof(Sigaction));
        act.sa_handler = SignalHandler::handleSignal;
        sigaction(SIGPIPE, &act, nullptr);
        sigaction(SIGHUP, &act, nullptr);
        sigaction(SIGINT, &act, nullptr);
        sigaction(SIGQUIT, &act, nullptr);
        sigaction(SIGTERM, &act, nullptr);
    }
}