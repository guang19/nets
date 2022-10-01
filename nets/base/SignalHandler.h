//
// Created by guang19
//

#ifndef NETS_SIGNAL_HANDLER_H
#define NETS_SIGNAL_HANDLER_H

#include <csignal>
#include <functional>

#include "nets/base/Noncopyable.h"

namespace nets
{
    class SignalHandler : Noncopyable
    {
    public:
        using SignoType = ::int32_t;
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
            if (gSignalHandler != nullptr)
            {
                gSignalHandler(signo, info, context);
            }
        }

    private:
        static SignalHandlerType gSignalHandler;
    };
} // namespace nets

#endif // NETS_SIGNAL_HANDLER_H