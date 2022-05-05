//
// Created by YangGuang on 2022/5/2.
//

#include "nets/base/Timestamp.h"

#include <sys/time.h>

namespace nets
{
	namespace base
	{
		Timestamp::Timestamp() : timestampSinceEpoch_(0)
		{
		}

		Timestamp::Timestamp(::std::time_t timestamp) : timestampSinceEpoch_(timestamp)
		{
		}

		Timestamp::Timestamp(::std::time_t secondsSinceEpoch, ::std::time_t microseconds)
			: Timestamp((secondsSinceEpoch * MicrosecondsPerSecond) + microseconds)
		{
		}

		Timestamp::Timestamp(const Timestamp& rhs)
		{
			timestampSinceEpoch_ = rhs.timestampSinceEpoch_;
		}

		Timestamp::Timestamp(Timestamp&& rhs) noexcept
		{
			timestampSinceEpoch_ = rhs.timestampSinceEpoch_;
			rhs.timestampSinceEpoch_ = 0;
		}

		Timestamp& Timestamp::operator=(const Timestamp& rhs)
		{
			if (this != &rhs)
			{
				timestampSinceEpoch_ = rhs.timestampSinceEpoch_;
			}
			return *this;
		}

		Timestamp& Timestamp::operator=(Timestamp&& rhs) noexcept
		{
			if (this != &rhs)
			{
				timestampSinceEpoch_ = rhs.timestampSinceEpoch_;
				rhs.timestampSinceEpoch_ = 0;
			}
			return *this;
		}

		Timestamp Timestamp::now()
		{
			struct timeval tmV {};
			::gettimeofday(&tmV, nullptr);
			return Timestamp(tmV.tv_sec, tmV.tv_usec);
		}

		::std::time_t Timestamp::getSecondsSinceEpoch() const
		{
			return static_cast<::std::time_t>(timestampSinceEpoch_ / MicrosecondsPerSecond);
		}

		uint32_t Timestamp::getMicroseconds() const
		{
			return static_cast<uint32_t>(timestampSinceEpoch_ % MicrosecondsPerSecond);
		}
	} // namespace base
} // namespace nets