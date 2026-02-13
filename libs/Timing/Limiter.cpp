#include "stdafx.h"
#include "Limiter.h"

#include "core/macros.h"

#include "rftl/thread"


namespace RF::time {
///////////////////////////////////////////////////////////////////////////////
namespace details {

void Limiter::Reset()
{
	mSpanStart = PerfClock::now();
	mSpanEnd = PerfClock::now();
}



CommonClock::duration Limiter::StallFor( CommonClock::duration desiredSpanTime )
{
	RF_ASSERT( desiredSpanTime.count() > 0 );

	PerfClock::time_point const naturalSpanEnd = PerfClock::now();
	CommonClock::duration const naturalSpanTime = naturalSpanEnd - mSpanStart;
	CommonClock::duration const timeRemaining = desiredSpanTime - naturalSpanTime;
	if( timeRemaining.count() > 0 )
	{
		rftl::this_thread::sleep_for( timeRemaining );
	}

	mSpanEnd = PerfClock::now();
	mSpanStart = PerfClock::now();

	return timeRemaining;
}

}
///////////////////////////////////////////////////////////////////////////////
}
