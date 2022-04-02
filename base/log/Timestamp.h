//
// Created by YangGuang on 2022/4/1.
//

#ifndef NETS_TIMESTAMP_H
#define NETS_TIMESTAMP_H

#include <chrono>
#include <string>
#include "base/Copyable.h"

namespace nets
{
	namespace base
	{
		class Timestamp : public Copyable
		{
			public:
				Timestamp() : microsecondsSingsEpoch_(0)
				{
				}

				explicit Timestamp(uint64_t microSecondsSingsEpoch) : microsecondsSingsEpoch_(microSecondsSingsEpoch)
				{
				}

				Timestamp(const Timestamp& rhs);
				Timestamp(Timestamp&& rhs) noexcept ;
				Timestamp& operator=(const Timestamp& rhs);
				Timestamp& operator=(Timestamp&& rhs) noexcept ;

				bool operator<(const Timestamp& rhs) const;
				bool operator>(const Timestamp& rhs) const;
				bool operator==(const Timestamp& rhs) const;
				bool operator!=(const Timestamp& rhs) const;
				bool operator>=(const Timestamp& rhs) const;
				bool operator<=(const Timestamp& rhs) const;
				explicit operator bool() const;

				Timestamp plusSeconds(int32_t seconds);
				Timestamp plusMinutes(int32_t minutes);
				Timestamp plusHours(int32_t hours);
				Timestamp plusDays(int32_t days);
				::std::string formatTime(bool showMicroseconds = false) const;

				static Timestamp fromUnixTime(::std::time_t seconds);
				static Timestamp fromUnixTime(::std::time_t seconds, uint32_t microseconds);
				static Timestamp now();

			private:
				uint32_t getMicroseconds() const
				{
					return static_cast<uint32_t>(microsecondsSingsEpoch_ % Timestamp::MicrosecondsPerSecond);
				}

				::std::time_t getSeconds() const
				{
					return static_cast<::std::time_t>(microsecondsSingsEpoch_ / Timestamp::MicrosecondsPerSecond);
				}

			private:
				uint64_t microsecondsSingsEpoch_;
				const static uint32_t MicrosecondsPerSecond = 1000000U;
		};
	} // namespace base
} // namespace nets

#endif //NETS_TIMESTAMP_H