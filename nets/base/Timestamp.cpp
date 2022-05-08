//
// Created by YangGuang on 2022/5/2.
//

#include "nets/base/Timestamp.h"

#include <sys/time.h>
#include <utility>

namespace nets
{
	namespace base
	{
		Timestamp::Timestamp() : timestampSinceEpoch_(0)
		{
		}

		Timestamp::Timestamp(TimeType timestamp) : timestampSinceEpoch_(timestamp)
		{
		}

		Timestamp::Timestamp(TimeType secondsSinceEpoch, uint32_t microseconds)
			: Timestamp((secondsSinceEpoch * MicrosecondsPerSecond) + microseconds)
		{
		}

		Timestamp::Timestamp(const Timestamp& other)
		{
			timestampSinceEpoch_ = other.timestampSinceEpoch_;
		}

		Timestamp::Timestamp(Timestamp&& other) noexcept
		{
			timestampSinceEpoch_ = other.timestampSinceEpoch_;
			other.timestampSinceEpoch_ = 0;
		}

		Timestamp& Timestamp::operator=(const Timestamp& other)
		{
			if (this != &other)
			{
				timestampSinceEpoch_ = other.timestampSinceEpoch_;
			}
			return *this;
		}

		Timestamp& Timestamp::operator=(Timestamp&& other) noexcept
		{
			if (this != &other)
			{
				timestampSinceEpoch_ = other.timestampSinceEpoch_;
				other.timestampSinceEpoch_ = 0;
			}
			return *this;
		}

		void Timestamp::swap(Timestamp &&other)
		{
			::std::swap(timestampSinceEpoch_, other.timestampSinceEpoch_);
		}

		Timestamp Timestamp::now()
		{
			struct timeval tmV {};
			::gettimeofday(&tmV, nullptr);
			return Timestamp(tmV.tv_sec, tmV.tv_usec);
		}
	} // namespace base
} // namespace nets