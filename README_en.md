# Nets
[中文](./README.md) | En

```
            _       
 _ __   ___| |_ ___ 
| '_ \ / _ \ __/ __|
| | | |  __/ |_\__ \
|_| |_|\___|\__|___/
```

nets is a non-blocking, multi-threaded network library for Linux based on the Reactor network model. With nets, users can easily build their own TCP/UDP network services.

## Env
- Developed based on Linux 5.10.16;
- GCC version is 9.4.0 or 12.2.1, Clang version is 10.0.0 or 15.0.7.

ArchLinux:
```shell
sudo pacman -S git gcc clang cmake make clang-format
```

Ubuntu:
```shell
sudo apt-get install git gcc clang cmake make clang-format
```

```git
git clone git@github.com:guang19/nets.git
or
git clone https://github.com/guang19/nets.git
```

Build:
```shell
cd nets/

mkdir -p build/debug

cmake ../../ -DCMAKE_C_COMPILER="/usr/bin/gcc" -DCMAKE_CXX_COMPILER="/usr/bin/g++"

make

make install
```

## Usage

### Build TCP services using Nets

TCP Server:
```c++
#include "nets/net/bootstrap/ServerBootstrap.h"

// The test code uses using namespace
using namespace nets;

// ChannelHandler that handles events on the server
class TestServerChannelHandler : public SocketChannelHandler
{
public:
	// Handle client connection events
    void channelConnect(SocketChannelContext& channelContext, const InetSockAddress& localAddress,
                        const InetSockAddress& peerAddress) override
    {
        NETS_SYSTEM_LOG_DEBUG << "isActive=" << channelContext.isActive();
        NETS_SYSTEM_LOG_DEBUG << "Server channelConnect ====local address:" << localAddress.toString()
                                   << " client address:" << peerAddress.toString();
        assert(!channelContext.channel().eventLoop()->parent()->isShutdown());
        fireChannelConnect(channelContext, localAddress, peerAddress);
    }

	// Handle client disconnect event
    void channelDisconnect(SocketChannelContext& channelContext) override
    {
        NETS_SYSTEM_LOG_DEBUG << "Server channelDisconnect:" << channelContext.peerAddress().toString();
        fireChannelDisconnect(channelContext);
    }
    
	// Handle messages sent by clients
    void channelRead(SocketChannelContext& channelContext, ByteBuffer& message) override
    {
        NETS_SYSTEM_LOG_DEBUG << "Server recv client message is:" << message.toString();
        //        channelContext.write(message);
        channelContext.write(message,
                             [this](SocketChannelContext& ctx)
                             {
                                 writeComplete(ctx);
                             });
        fireChannelRead(channelContext, message);
    }
    
	// write completion callback
    void writeComplete(SocketChannelContext& channelContext)
    {
        NETS_SYSTEM_LOG_DEBUG << "Server writeComplete";
    }
};

// Start the server
int main(int argc, char** argv)
{
    ServerBootstrap(8)
        .option(SockOption::SNDBUF, 1024)
        .option(SockOption::RCVBUF, 1024)
        .option(SockOption::BACKLOG, 1028)
        // Add a global ChannelHandler, generally do not use this method to add
        //        .childHandler(::std::shared_ptr<SocketChannelHandler>(nullptr))
        .childHandler(
            [](SocketChannel& channel)
            {
                channel.pipeline().addLast(::std::shared_ptr<SocketChannelHandler>(new TestServerChannelHandler()));
            })
        .bind(8080)
        .sync();
	return 0;
}
```

TCP Client:
```c++
#include "nets/net/bootstrap/Bootstrap.h"

// The test code uses using namespace
using namespace nets;

// The ChannelHandler that the client handles events
class TestClientChannelHandler : public SocketChannelHandler
{
public:
	// Client connection server event
    void channelConnect(SocketChannelContext& channelContext, const InetSockAddress& localAddress,
                        const InetSockAddress& peerAddress) override
    {
        NETS_SYSTEM_LOG_DEBUG << "isActive=" << channelContext.isActive();
        NETS_SYSTEM_LOG_DEBUG << "Client channelConnect ====local address:" << localAddress.toString()
                                   << " server address:" << peerAddress.toString();
        channelContext.write("Hello Server",
                             [this](SocketChannelContext& ctx)
                             {
                                 writeComplete(ctx);
                             });
        fireChannelConnect(channelContext, localAddress, peerAddress);
    }
    
	// Client and server connection events
    void channelDisconnect(SocketChannelContext& channelContext) override
    {
        NETS_SYSTEM_LOG_DEBUG << "Client channelDisconnect:" << channelContext.peerAddress().toString();
        fireChannelDisconnect(channelContext);
    }

	// Receive response from server
    void channelRead(SocketChannelContext& channelContext, ByteBuffer& message) override
    {
        NETS_SYSTEM_LOG_DEBUG << "Client recv server message is:" << message.toString();
        fireChannelRead(channelContext, message);
    }
    
	// write completion callback
    void writeComplete(SocketChannelContext& channelContext)
    {
        NETS_SYSTEM_LOG_DEBUG << "Client writeComplete";
    }
};

// Start the client
int main(int argc, char** argv)
{
    Bootstrap()
        .option(SockOption::SNDBUF, 1024)
        .option(SockOption::RCVBUF, 1024)
        .retry(true, 3000)
        //        .channelHandler(::std::shared_ptr<SocketChannelHandler>(nullptr))
        .channelHandler(
            [](SocketChannel& channel)
            {
                channel.pipeline().addLast(::std::shared_ptr<SocketChannelHandler>(new TestClientChannelHandler()));
            })
        .connect("127.0.0.1", 8080)
        .sync();
	return 0;
}
```


### Build UDP service using Nets
Nets supports UDP unicast, multicast and broadcast modes. The following uses unicast as an example to build UDP network services.

Recipient:
```c++
#include "nets/net/bootstrap/Bootstrap.h"

// The test code uses using namespace
using namespace nets;

// ChannelHandler for recipient
class TestUdpRecipientHandler : public DatagramChannelHandler
{
public:
	// Handle the UDP service startup success event
    void channelActive(DatagramChannelContext& channelContext) override
    {
        NETS_SYSTEM_LOG_DEBUG << "TestUdpRecipientHandler::channelActive";
    }
    
	// Process the data sent by the UDP sender
    void channelRead(DatagramChannelContext& channelContext, DatagramPacket& message) override
    {
        NETS_SYSTEM_LOG_DEBUG << "TestUdpRecipientHandler::channelRead recv from " << message.recipient().toString()
                                   << "\nmessage is:" << message.byteBuffer().toString();
        channelContext.write("Hello UdpSender", InetSockAddress("127.0.0.1", 8081));
    }
};

// Start the UDP receiver
int main(int argc, char** argv)
{
    Bootstrap()
        .option(SockOption::BROADCAST, true)
        .channelHandler(
            [](DatagramChannel& channel)
            {
                channel.pipeline().addLast(::std::shared_ptr<DatagramChannelHandler>(new TestUdpRecipientHandler));
            })
        .bind(8080)
        .sync();
	return 0;
}
```

Sender:
```c++
#include "nets/net/bootstrap/Bootstrap.h"

// The test code uses using namespace
using namespace nets;

// ChannelHandler for sender
class TestUdpSenderHandler : public DatagramChannelHandler
{
public:
	// Handle the UDP service startup success event
    void channelActive(DatagramChannelContext& channelContext) override
    {
        NETS_SYSTEM_LOG_DEBUG << "TestUdpSenderHandler::channelActive";
        channelContext.write("Hello UdpRecipient", InetSockAddress("127.0.0.1", 8080));
    }

	// Process the data that the UDP receiver responds to
    void channelRead(DatagramChannelContext& channelContext, DatagramPacket& message) override
    {
        NETS_SYSTEM_LOG_DEBUG << "TestUdpSenderHandler::channelRead recv from " << message.recipient().toString()
                                   << "\nmessage is:" << message.byteBuffer().toString();
    }
};

// Start the UDP sender
int main(int argc, char** argv)
{
    Bootstrap()
        .option(SockOption::BROADCAST, true)
        .channelHandler(
            [](DatagramChannel& channel)
            {
                channel.pipeline().addLast(::std::shared_ptr<DatagramChannelHandler>(new TestUdpSenderHandler));
            })
        .bind(8081)
        .sync();
	return 0;
}
```

The above are a few simple demos. In addition, Nets also supports features such as scheduled tasks. You can find the corresponding test cases in the [tests directory](./nets/net/tests) to try.

### TODO
- [ ] Implement Http and other network protocols based on Nets;
- [ ] Improve Nets documentation.

**All students are welcome to contribute to this project and fix bugs.**
