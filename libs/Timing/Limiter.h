#pragma once
#include "project.h"

#include "Timing/TimeFwd.h"

#include "core_time/CommonClock.h"
#include "core_time/PerfClock.h"

namespace RF::time {
///////////////////////////////////////////////////////////////////////////////
namespace details {

struct TIMING_API Limiter
{
public:
	void Reset();
	CommonClock::duration GetLastNonStallDuration() const;

protected:
	CommonClock::duration StallToMatch( CommonClock::duration desiredSpanTime );

private:
	PerfClock::time_point mSpanStart;
	PerfClock::time_point mSpanEnd;
	PerfClock::duration mLastNonStallDuration;
};

}
///////////////////////////////////////////////////////////////////////////////

template<typename ChronoDurationType, long long DesiredTimeSpan>
struct StaticLimiter : public details::Limiter
{
	static constexpr ChronoDurationType kSpanDuration = ChronoDurationType( DesiredTimeSpan );

	CommonClock::duration Stall()
	{
		return StallToMatch( kSpanDuration );
	}

	CommonClock::duration AdjustedStall( CommonClock::duration adjust )
	{
		return StallToMatch( kSpanDuration + adjust );
	}
};

///////////////////////////////////////////////////////////////////////////////
}
