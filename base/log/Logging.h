//
// Created by YangGuang on 2022/4/1.
//

#ifndef NETS_LOGGING_H
#define NETS_LOGGING_H

namespace nets
{
	namespace base
	{
		class Logger
		{
			public:
				enum LogLevel
				{
					TRACE = 0,
					DEBUG,
					INFO,
					WARN,
					ERROR,
					FATAL
				};
		};
	} // namespace base
} // namespace nets

#endif //NETS_LOGGING_H
