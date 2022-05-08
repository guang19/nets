//
// Created by YangGuang on 2022/5/2.
//

#ifndef NETS_TIMESTAMP_H
#define NETS_TIMESTAMP_H

#include <cstdint>
#include <ctime>
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
				using TimeTp = ::time_t;
				
			public:
				Timestamp();
				explicit Timestamp(TimeTp timestamp);
				explicit Timestamp(TimeTp secondsSinceEpoch, uint32_t microseconds);

				Timestamp(const Timestamp& other);
				Timestamp(Timestamp&& other) noexcept;

				Timestamp& operator=(const Timestamp& other);
				Timestamp& operator=(Timestamp&& other) noexcept;

				void swap(Timestamp&& other);

			public:
				static Timestamp now();

				inline TimeTp getTimestamp() const
				{
					return timestampSinceEpoch_;
				}

				inline TimeTp getSecondsSinceEpoch() const
				{
					return static_cast<TimeTp>(timestampSinceEpoch_ / MicrosecondsPerSecond);
				}

				inline uint32_t getMicroseconds() const
				{
					return static_cast<uint32_t>(timestampSinceEpoch_ % MicrosecondsPerSecond);
				}

			private:
				TimeTp timestampSinceEpoch_ { 0 };
		};
	} // namespace base
} // namespace nets

#endif //NETS_TIMESTAMP_H
