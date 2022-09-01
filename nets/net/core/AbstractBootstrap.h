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
        using ChannelOptionList = ::std::vector<ChannelOption>;

    public:
        explicit AbstractBootstrap() : channelOptions_(), mainLoopGroup_(NumbOfMainEventLoops, MainEventLoopGroupName)
        {
            SignalHandler::initSignalHandler(
                ::std::bind(&AbstractBootstrap<B>::handleSignal, this, ::std::placeholders::_1));
        }

        virtual ~AbstractBootstrap() = default;

    public:
        B& option(const ChannelOption& channelOption, const ChannelOption::ValueType& value)
        {
            channelOptions_.emplace_back(channelOption.sockOpt(), value);
            return static_cast<B&>(*this);
        }

        virtual void shutdown() = 0;
        virtual bool isShutdown() const = 0;

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
                    if (!isShutdown())
                    {
                        shutdown();
                    }
                    break;
                }
            }
        }

    protected:
        ChannelOptionList channelOptions_ {};
        EventLoopGroup mainLoopGroup_;

    private:
        static constexpr IntType NumbOfMainEventLoops = 1;
        static constexpr char MainEventLoopGroupName[] = "MainLoopGroup";
    };
} // namespace nets::net

#endif // NETS_NET_ABSTRACT_BOOTSTRAP_H