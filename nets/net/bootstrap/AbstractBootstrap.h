//
// Created by guang19
//

#ifndef NETS_ABSTRACT_BOOTSTRAP_H
#define NETS_ABSTRACT_BOOTSTRAP_H

#include "nets/base/exception/SegmentationFaultException.h"
#include "nets/base/SignalHandler.h"
#include "nets/net/core/ChannelOption.h"
#include "nets/net/core/EventLoopGroup.h"

namespace nets
{
    template <class B>
    class AbstractBootstrap : Noncopyable
    {
    public:
        using IntType = typename EventLoopGroup::IntType;
        using StringType = ::std::string;
        using ChannelOptionList = Channel::ChannelOptionList;

    public:
        explicit AbstractBootstrap() : channelOptions_(), mainLoopGroup_(gNumbOfMainEventLoops, gMainEventLoopGroupName)
        {
            SignalHandler::initSignalHandler(::std::bind(&AbstractBootstrap<B>::handleSignal, this, ::std::placeholders::_1,
                                                         ::std::placeholders::_2, ::std::placeholders::_3));
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
        void handleSignal(SignalHandler::SignoType signo, SignalHandler::SigInfo* info, void* context)
        {
            UNUSED(context);
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
                case SIGSEGV:
                {
                    LOGS_ERROR << "handleSignal SIGSEGV backtrace info:\n" << STACK_TRACE;
                    THROW_FMT(SegmentationFaultException, "segmentation fault,errno=%d,si_uid=%d,si_pid=%d", info->si_errno,
                              info->si_uid, info->si_pid);
                    break;
                }
            }
        }

    protected:
        ChannelOptionList channelOptions_;
        EventLoopGroup mainLoopGroup_;

    private:
        static constexpr IntType gNumbOfMainEventLoops = 1;
        static constexpr char gMainEventLoopGroupName[] = "MainLoopGroup";
    };
} // namespace nets

#endif // NETS_ABSTRACT_BOOTSTRAP_H