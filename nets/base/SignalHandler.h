//
// Created by guang19
//

#ifndef NETS_BASE_SIGNAL_HANDLER_H
#define NETS_BASE_SIGNAL_HANDLER_H

#include <functional>
#include <csignal>

#include "nets/base/Noncopyable.h"

namespace nets::base
{
    class SignalHandler : Noncopyable
    {
    public:
        using SignoType = ::int32_t;
        using Sigaction = struct sigaction;
        using SignalHandlerType = ::std::function<void(SignoType signo)>;

    public:
        explicit SignalHandler() = default;
        ~SignalHandler() = default;

    public:
        static void initSignalHandler(const SignalHandlerType& signalHandler);

        static void handleSignal(SignoType signo)
        {
            if (gSignalHandler != nullptr)
            {
                gSignalHandler(signo);
            }
        }

    private:
        static SignalHandlerType gSignalHandler;
    };
} // namespace nets::base

#endif // NETS_BASE_SIGNAL_HANDLER_H