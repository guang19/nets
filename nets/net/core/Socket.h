//
// Created by n021949 on 2022/5/13.
//

#ifndef NETS_SOCKET_H
#define NETS_SOCKET_H

#include "nets/base/Copyable.h"
#include "nets/base/Noncopyable.h"
#include "nets/net/core/NetUtil.h"
#include "nets/net/core/InetAddress.h"

namespace nets::net
{
	namespace socket
	{
		// usually, newer os all support dynamic sock buffer resizing
		// dont require manual set
		void setSendBuf(SockLenType sendBufLen = 64 * 1024);
		void setRecvBuf(SockLenType recvBufLen = 64 * 1024);

		void setAddrReuse(bool enable = true);
		void setPortReuse(bool enable = true);
		void setKeepAlive(bool enable = true);
		void setTcpNoDelay(bool enable = true);
		void setNonBlock(bool enable = true);
	};
} // namespace nets::net

#endif // NETS_SOCKET_H
