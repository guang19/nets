//
// Created by YangGuang on 2022/5/2.
//

#ifndef NETS_TIMESTAMP_H
#define NETS_TIMESTAMP_H

#include <ctime>
#include <cstdint>
#include "nets/base/Copyable.h"

namespace nets
{
	namespace base
	{
		namespace
		{
			constexpr uint32_t MillisecondsPerSecond = 1000U;
			constexpr uint32_t MicrosecondsPerSecond = 1000000U;
			constexpr uint32_t NanosecondsPerSecond = 1000000000U;
		}

		class Timestamp : public Copyable
		{
			public:
				Timestamp();
				explicit Timestamp(::std::time_t timestamp);
				explicit Timestamp(::std::time_t secondsSinceEpoch, ::std::time_t microseconds);

				Timestamp(const Timestamp& rhs);
				Timestamp(Timestamp&& rhs) noexcept;

				Timestamp& operator=(const Timestamp& rhs);
				Timestamp& operator=(Timestamp&& rhs) noexcept;

			public:
				static Timestamp now();

				inline ::std::time_t getTimestamp() const
				{
					return timestampSinceEpoch_;
				}

				::std::time_t getSecondsSinceEpoch() const;
				uint32_t getMicroseconds() const;

			private:
				::std::time_t timestampSinceEpoch_ { 0 };
		};
	} // namespace base
} // namespace nets

#endif //NETS_TIMESTAMP_H
