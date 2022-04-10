//
// Created by YangGuang on 2022/4/1.
//

#include "base/log/Timestamp.h"

namespace nets
{
	namespace base
	{
		namespace
		{
			constexpr uint32_t MicrosecondsPerSecond = 1000000U;
		}

		Timestamp::Timestamp(const Timestamp& rhs)
		{
			microsecondsSingsEpoch_ = rhs.microsecondsSingsEpoch_;
		}

		Timestamp::Timestamp(Timestamp&& rhs) noexcept
		{
			microsecondsSingsEpoch_ = rhs.microsecondsSingsEpoch_;
			rhs.microsecondsSingsEpoch_ = 0;
		}

		Timestamp& Timestamp::operator=(const Timestamp& rhs)
		{
			if (rhs && this != &rhs)
			{
				microsecondsSingsEpoch_ = rhs.microsecondsSingsEpoch_;
			}
			return *this;
		}

		Timestamp& Timestamp::operator=(Timestamp&& rhs) noexcept
		{
			if (rhs && this != &rhs)
			{
				microsecondsSingsEpoch_ = rhs.microsecondsSingsEpoch_;
				rhs.microsecondsSingsEpoch_ = 0;
			}
			return *this;
		}

		Timestamp::operator bool() const
		{
			return microsecondsSingsEpoch_ > 0;
		}

		bool Timestamp::operator<(const Timestamp &rhs) const
		{
			return microsecondsSingsEpoch_ < rhs.microsecondsSingsEpoch_;
		}

		bool Timestamp::operator>(const Timestamp &rhs) const
		{
			return microsecondsSingsEpoch_ > rhs.microsecondsSingsEpoch_;
		}

		bool Timestamp::operator==(const Timestamp &rhs) const
		{
			return microsecondsSingsEpoch_ == rhs.microsecondsSingsEpoch_;
		}

		bool Timestamp::operator!=(const Timestamp& rhs) const
		{
			return microsecondsSingsEpoch_ != rhs.microsecondsSingsEpoch_;
		}

		bool Timestamp::operator>=(const Timestamp& rhs) const
		{
			return microsecondsSingsEpoch_ >= rhs.microsecondsSingsEpoch_;
		}

		bool Timestamp::operator<=(const Timestamp& rhs) const
		{
			return microsecondsSingsEpoch_ <= rhs.microsecondsSingsEpoch_;
		}

		Timestamp Timestamp::plusSeconds(int32_t seconds)
		{
			uint32_t microseconds = getMicroseconds();
			::std::time_t calculateSeconds = getSeconds() + seconds;
			return Timestamp(static_cast<uint64_t>(calculateSeconds) * MicrosecondsPerSecond + microseconds);
		}

		Timestamp Timestamp::plusMinutes(int32_t minutes)
		{
			uint32_t microseconds = getMicroseconds();
			::std::time_t calculateSeconds = getSeconds() + (minutes * 60);
			return Timestamp(static_cast<uint64_t>(calculateSeconds) * MicrosecondsPerSecond + microseconds);
		}

		Timestamp Timestamp::plusHours(int32_t hours)
		{
			uint32_t microseconds = getMicroseconds();
			::std::time_t calculateSeconds = getSeconds() + (hours * 3600);
			return Timestamp(static_cast<uint64_t>(calculateSeconds) * MicrosecondsPerSecond + microseconds);
		}

		Timestamp Timestamp::plusDays(int32_t days)
		{
			uint32_t microseconds = getMicroseconds();
			::std::time_t calculateSeconds = getSeconds() + (days * 86400);
			return Timestamp(static_cast<uint64_t>(calculateSeconds) * MicrosecondsPerSecond + microseconds);
		}

		::std::string Timestamp::formatTime(bool showMicroseconds) const
		{
			::std::time_t seconds = getSeconds();
			struct tm tmS {};
			localtime_r(&seconds, &tmS);
			if (showMicroseconds)
			{
				char timeStr[27] = {0};
				snprintf(timeStr, sizeof(timeStr), "%04d-%02d-%02d %02d:%02d:%02d.%06d",
						 tmS.tm_year + 1900, tmS.tm_mon, tmS.tm_mday, tmS.tm_hour, tmS.tm_min, tmS.tm_sec,
						 getMicroseconds());
				return timeStr;
			}
			else
			{
				char timeStr[20] = {0};
				snprintf(timeStr, sizeof(timeStr), "%04d-%02d-%02d %02d:%02d:%02d",
						 tmS.tm_year + 1900, tmS.tm_mon, tmS.tm_mday, tmS.tm_hour, tmS.tm_min, tmS.tm_sec);
				return timeStr;
			}
		}

		Timestamp Timestamp::fromUnixTime(::std::time_t seconds)
		{
			return fromUnixTime(seconds, 0);
		}

		Timestamp Timestamp::fromUnixTime(::std::time_t seconds, uint32_t microseconds)
		{
			return Timestamp(static_cast<uint64_t>(seconds) * MicrosecondsPerSecond + microseconds);
		}

		Timestamp Timestamp::now()
		{
			return Timestamp(::std::chrono::duration_cast<::std::chrono::microseconds>(
				::std::chrono::system_clock::now().time_since_epoch()).count());
		}

		uint32_t Timestamp::getMicroseconds() const
		{
			return static_cast<uint32_t>(microsecondsSingsEpoch_ % MicrosecondsPerSecond);
		}

		::std::time_t Timestamp::getSeconds() const
		{
			return static_cast<::std::time_t>(microsecondsSingsEpoch_ / MicrosecondsPerSecond);
		}
	} // namespace base
} // namespace nets
