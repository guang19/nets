//
// Created by YangGuang on 2022/4/1.
//

#include "Timestamp.h"
#include <iomanip>
#include <sstream>

namespace nets
{
	namespace base
	{

		Timestamp::Timestamp(const Timestamp& rhs)
		{
			microSecondsSingsEpoch_ = rhs.microSecondsSingsEpoch_;
		}

		Timestamp::Timestamp(Timestamp&& rhs) noexcept
		{
			microSecondsSingsEpoch_ = rhs.microSecondsSingsEpoch_;
			rhs.microSecondsSingsEpoch_ = 0;
		}

		Timestamp& Timestamp::operator=(const Timestamp& rhs)
		{
			if (rhs && this != &rhs)
			{
				microSecondsSingsEpoch_ = rhs.microSecondsSingsEpoch_;
			}
			return *this;
		}

		Timestamp& Timestamp::operator=(Timestamp&& rhs) noexcept
		{
			if (rhs && this != &rhs)
			{
				microSecondsSingsEpoch_ = rhs.microSecondsSingsEpoch_;
				rhs.microSecondsSingsEpoch_ = 0;
			}
			return *this;
		}

		bool Timestamp::operator<(const Timestamp &rhs) const
		{
			return microSecondsSingsEpoch_ < rhs.microSecondsSingsEpoch_;
		}

		bool Timestamp::operator>(const Timestamp &rhs) const
		{
			return microSecondsSingsEpoch_ > rhs.microSecondsSingsEpoch_;
		}

		bool Timestamp::operator==(const Timestamp &rhs) const
		{
			return microSecondsSingsEpoch_ == rhs.microSecondsSingsEpoch_;
		}

		bool Timestamp::operator!=(const Timestamp& rhs) const
		{
			return microSecondsSingsEpoch_ != rhs.microSecondsSingsEpoch_;
		}

		bool Timestamp::operator>=(const Timestamp& rhs) const
		{
			return microSecondsSingsEpoch_ >= rhs.microSecondsSingsEpoch_;
		}

		bool Timestamp::operator<=(const Timestamp& rhs) const
		{
			return microSecondsSingsEpoch_ <= rhs.microSecondsSingsEpoch_;
		}

		Timestamp::operator bool() const
		{
			return microSecondsSingsEpoch_ > 0;
		}

		::std::string Timestamp::formatTime(const char* formatter) const
		{
			::std::time_t seconds = static_cast<::std::time_t>(microSecondsSingsEpoch_ / Timestamp::MicroSecondsPerSecond);
			::std::stringstream ss;
			ss << ::std::put_time(localtime(&seconds), formatter);
			return ss.str();
		}

		Timestamp Timestamp::fromUnixTime(::std::time_t seconds)
		{
			return fromUnixTime(seconds, 0);
		}

		Timestamp Timestamp::fromUnixTime(::std::time_t seconds, uint32_t microSeconds)
		{
			return Timestamp(static_cast<uint64_t>(seconds) * Timestamp::MicroSecondsPerSecond + microSeconds);
		}

		Timestamp Timestamp::now()
		{
			return Timestamp(::std::chrono::duration_cast<::std::chrono::microseconds>(
				::std::chrono::system_clock::now().time_since_epoch()).count());
		}

	} // namespace base
} // namespace nets
