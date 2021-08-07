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

protected:
	CommonClock::duration StallFor( CommonClock::duration desiredSpanTime );

private:
	PerfClock::time_point m_SpanStart;
	PerfClock::time_point m_SpanEnd;
};

}
///////////////////////////////////////////////////////////////////////////////

template<typename ChronoDurationType, long long DesiredTimeSpan>
struct StaticLimiter : public details::Limiter
{
	static constexpr ChronoDurationType kSpanDuration = ChronoDurationType( DesiredTimeSpan );

	CommonClock::duration Stall()
	{
		return StallFor( kSpanDuration );
	}

	CommonClock::duration AdjustedStall( CommonClock::duration adjust )
	{
		return StallFor( kSpanDuration + adjust );
	}
};

///////////////////////////////////////////////////////////////////////////////
}
