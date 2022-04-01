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
				Timestamp() : microSecondsSingsEpoch_(0)
				{
				}

				explicit Timestamp(uint64_t microSecondsSingsEpoch) : microSecondsSingsEpoch_(microSecondsSingsEpoch)
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
//				Timestamp operator+(const Timestamp& rhs);
//				Timestamp operator-(const Timestamp& rhs);

				::std::string formatTime(const char* formatter = DefaultDateTimeFormatter) const;

				static Timestamp fromUnixTime(::std::time_t seconds);
				static Timestamp fromUnixTime(::std::time_t seconds, uint32_t microSeconds);
				static Timestamp now();

			private:
				uint64_t microSecondsSingsEpoch_;
				const static uint32_t MicroSecondsPerSecond;
				const static char* DefaultDateTimeFormatter;
		};
		const uint32_t Timestamp::MicroSecondsPerSecond = 1000000U;
		const char* Timestamp::DefaultDateTimeFormatter = "%Y-%m-%d %H:%M:%S";
	} // namespace base
} // namespace nets

#endif //NETS_TIMESTAMP_H
