//
// Created by guang19
//

#ifndef NETS_NET_ABSTRACT_BOOTSTRAP_H
#define NETS_NET_ABSTRACT_BOOTSTRAP_H

#include "nets/base/SignalHandler.h"
#include "nets/net/core/ChannelOption.h"
#include "nets/net/core/EventLoopGroup.h"

namespace nets::net
{
    template <class B>
    class AbstractBootstrap : nets::base::Noncopyable
    {
    public:
        using IntType = typename EventLoopGroup::IntType;
        using SignalHandler = typename nets::base::SignalHandler;
        using ChannelOptionList = Channel::ChannelOptionList;

    public:
        explicit AbstractBootstrap() : channelOptions_(), mainLoopGroup_(gNumbOfMainEventLoops, gMainEventLoopGroupName)
        {
            SignalHandler::initSignalHandler(
                ::std::bind(&AbstractBootstrap<B>::handleSignal, this, ::std::placeholders::_1));
        }

        virtual ~AbstractBootstrap() = default;

    public:
        B& option(SockOption sockOption, const ChannelOption::ValueType& value)
        {
            channelOptions_[sockOption] = value;
            return static_cast<B&>(*this);
        }

        virtual void shutdown()
        {
            if (!mainLoopGroup_.isShutdown())
            {
                mainLoopGroup_.shutdown();
            }
        }

        virtual bool isShutdown() const
        {
            return mainLoopGroup_.isShutdown();
        }

    private:
        void handleSignal(SignalHandler::SignoType signo)
        {
            switch (signo)
            {
                case SIGPIPE:
                    break;
                case SIGHUP:
                case SIGINT:
                case SIGQUIT:
                case SIGTERM:
                {
                    shutdown();
                    break;
                }
            }
        }

    protected:
        ChannelOptionList channelOptions_ {};
        EventLoopGroup mainLoopGroup_;

    private:
        static constexpr IntType gNumbOfMainEventLoops = 1;
        static constexpr char gMainEventLoopGroupName[] = "MainLoopGroup";
    };
} // namespace nets::net

#endif // NETS_NET_ABSTRACT_BOOTSTRAP_H