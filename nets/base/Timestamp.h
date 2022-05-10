//
// Created by YangGuang on 2022/5/2.
//

#ifndef NETS_TIMESTAMP_H
#define NETS_TIMESTAMP_H

#include <cstdint>
#include <ctime>

#include "nets/base/Copyable.h"

namespace nets::base
{
	namespace
	{
		constexpr uint32_t MillisecondsPerSecond = 1000U;
		constexpr uint32_t MicrosecondsPerSecond = 1000000U;
	} // namespace

	class Timestamp : public Copyable
	{
	public:
		using TimeType = ::time_t;

	public:
		Timestamp();

		explicit Timestamp(TimeType timestamp);

		explicit Timestamp(TimeType secondsSinceEpoch, uint32_t microseconds);

		Timestamp(const Timestamp& other);

		Timestamp(Timestamp&& other) noexcept;

		Timestamp& operator=(const Timestamp& other);

		Timestamp& operator=(Timestamp&& other) noexcept;

		void swap(Timestamp&& other);

	public:
		static Timestamp now();

		inline TimeType timestamp() const
		{
			return timestampSinceEpoch_;
		}

		inline TimeType secondsSinceEpoch() const
		{
			return static_cast<TimeType>(timestampSinceEpoch_ / MicrosecondsPerSecond);
		}

		inline uint32_t microseconds() const
		{
			return static_cast<uint32_t>(timestampSinceEpoch_ % MicrosecondsPerSecond);
		}

	private:
		TimeType timestampSinceEpoch_ {0};
	};
} // namespace nets::base

#endif // NETS_TIMESTAMP_H
