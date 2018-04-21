#pragma once
#include "Timing/clocks.h"

namespace RF { namespace time {
///////////////////////////////////////////////////////////////////////////////

template<typename ChronoDurationType, long long DesiredTimeSpan>
struct Limiter
{
	void Reset()
	{
		m_SpanStart = RF::time::PerfClock::now();
		m_SpanEnd = RF::time::PerfClock::now();
	}

	void Stall()
	{
		RF::time::PerfClock::duration const desiredSpanTime = ChronoDurationType( DesiredTimeSpan );

		time::PerfClock::time_point const naturalSpanEnd = time::PerfClock::now();
		time::PerfClock::duration const naturalSpanTime = naturalSpanEnd - m_SpanStart;
		if( naturalSpanTime < desiredSpanTime )
		{
			time::PerfClock::duration const timeRemaining = desiredSpanTime - naturalSpanTime;
			rftl::this_thread::sleep_for( timeRemaining );
		}
		m_SpanEnd = time::PerfClock::now();
		time::PerfClock::duration const spanTime = m_SpanEnd - m_SpanStart;
		time::FrameClock::add_time( spanTime );
		m_SpanStart = time::PerfClock::now();
	}

	RF::time::PerfClock::time_point m_SpanStart;
	RF::time::PerfClock::time_point m_SpanEnd;
};

///////////////////////////////////////////////////////////////////////////////
}}
