# Nets
中文 | [En](./README_en.md)

```
            _       
 _ __   ___| |_ ___ 
| '_ \ / _ \ __/ __|
| | | |  __/ |_\__ \
|_| |_|\___|\__|___/
```

nets是一个Linux下非阻塞的、多线程网络库，基于Reactor网络模型编写，用户基于nets很轻松的搭建属于自己的 TCP/UDP 网络服务。

## 环境
- 基于Linux 5.10.16版本开发；
- GCC版本为9.4.0或12.2.1，Clang版本为10.0.0或15.0.7。

ArchLinux：
```shell
sudo pacman -S git gcc clang cmake make clang-format
```

Ubuntu：
```shell
sudo apt-get install git gcc clang cmake make clang-format
```

```git
git clone git@github.com:guang19/nets.git
或
git clone https://github.com/guang19/nets.git
```

构建：
```shell
cd nets/

mkdir -p build/debug

cd build/debug

cmake ../../ -DCMAKE_C_COMPILER="/usr/bin/gcc" -DCMAKE_CXX_COMPILER="/usr/bin/g++"

make

make install
```

## 使用

### 使用Nets构建TCP服务

服务端：
```c++
#include "nets/net/bootstrap/ServerBootstrap.h"

// test代码使用using namespace
using namespace nets;

// 服务端处理事件的ChannelHandler
class TestServerChannelHandler : public SocketChannelHandler
{
public:
	// 处理客户端连接事件
    void channelConnect(SocketChannelContext& channelContext, const InetSockAddress& localAddress,
                        const InetSockAddress& peerAddress) override
    {
        NETS_SYSTEM_LOG_DEBUG << "isActive=" << channelContext.isActive();
        NETS_SYSTEM_LOG_DEBUG << "Server channelConnect ====local address:" << localAddress.toString()
                                   << " client address:" << peerAddress.toString();
        assert(!channelContext.channel().eventLoop()->parent()->isShutdown());
        fireChannelConnect(channelContext, localAddress, peerAddress);
    }

	// 处理客户端断开连接事件
    void channelDisconnect(SocketChannelContext& channelContext) override
    {
        NETS_SYSTEM_LOG_DEBUG << "Server channelDisconnect:" << channelContext.peerAddress().toString();
        fireChannelDisconnect(channelContext);
    }
    
	// 处理客户端发送的消息
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
    
	// write完成回调
    void writeComplete(SocketChannelContext& channelContext)
    {
        NETS_SYSTEM_LOG_DEBUG << "Server writeComplete";
    }
};

// 启动服务端
int main(int argc, char** argv)
{
    ServerBootstrap(8)
        .option(SockOption::SNDBUF, 1024)
        .option(SockOption::RCVBUF, 1024)
        .option(SockOption::BACKLOG, 1028)
        // 添加全局的ChannelHandler，一般不使用这种方法添加
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

客户端：
```c++
#include "nets/net/bootstrap/Bootstrap.h"

// test代码使用using namespace
using namespace nets;

// 客户端处理事件的ChannelHandler
class TestClientChannelHandler : public SocketChannelHandler
{
public:
	// 客户端连接服务端事件
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
    
	// 客户端与服务端连接事件
    void channelDisconnect(SocketChannelContext& channelContext) override
    {
        NETS_SYSTEM_LOG_DEBUG << "Client channelDisconnect:" << channelContext.peerAddress().toString();
        fireChannelDisconnect(channelContext);
    }

	// 接收来自服务端的响应
    void channelRead(SocketChannelContext& channelContext, ByteBuffer& message) override
    {
        NETS_SYSTEM_LOG_DEBUG << "Client recv server message is:" << message.toString();
        fireChannelRead(channelContext, message);
    }
    
	// write完成回调
    void writeComplete(SocketChannelContext& channelContext)
    {
        NETS_SYSTEM_LOG_DEBUG << "Client writeComplete";
    }
};

// 启动客户端
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


### 使用Nets构建UDP服务
Nets支持UDP的单播，多播和广播模式，下面以单播为例构建UDP网络服务。

接收端：
```c++
#include "nets/net/bootstrap/Bootstrap.h"

// test代码使用using namespace
using namespace nets;

// 接收端处理事件的ChannelHandler
class TestUdpRecipientHandler : public DatagramChannelHandler
{
public:
	// 处理UDP服务启动成功事件
    void channelActive(DatagramChannelContext& channelContext) override
    {
        NETS_SYSTEM_LOG_DEBUG << "TestUdpRecipientHandler::channelActive";
    }
    
	// 处理UDP发送端发送的数据
    void channelRead(DatagramChannelContext& channelContext, DatagramPacket& message) override
    {
        NETS_SYSTEM_LOG_DEBUG << "TestUdpRecipientHandler::channelRead recv from " << message.recipient().toString()
                                   << "\nmessage is:" << message.byteBuffer().toString();
        channelContext.write("Hello UdpSender", InetSockAddress("127.0.0.1", 8081));
    }
};

// 启动UDP接收端
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

发送端：
```c++
#include "nets/net/bootstrap/Bootstrap.h"

// test代码使用using namespace
using namespace nets;

// 发送端处理事件的ChannelHandler
class TestUdpSenderHandler : public DatagramChannelHandler
{
public:
	// 处理UDP服务启动成功事件
    void channelActive(DatagramChannelContext& channelContext) override
    {
        NETS_SYSTEM_LOG_DEBUG << "TestUdpSenderHandler::channelActive";
        channelContext.write("Hello UdpRecipient", InetSockAddress("127.0.0.1", 8080));
    }

	// 处理UDP接收端响应的数据
    void channelRead(DatagramChannelContext& channelContext, DatagramPacket& message) override
    {
        NETS_SYSTEM_LOG_DEBUG << "TestUdpSenderHandler::channelRead recv from " << message.recipient().toString()
                                   << "\nmessage is:" << message.byteBuffer().toString();
    }
};

// 启动UDP发送端
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

以上是几个简单的Demo，除此之外，Nets还支持定时任务等特性，可以在[tests目录下](./nets/net/tests)找到对应的测试用例进行尝试。

### TODO
- [ ] 基于Nets实现Http等网络协议；
- [ ] 完善Nets文档。

**欢迎各位同学为本项目添砖加瓦修Bug。**
