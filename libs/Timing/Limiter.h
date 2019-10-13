#pragma once
#include "core_time/PerfClock.h"

#include "rftl/thread"

namespace RF { namespace time {
///////////////////////////////////////////////////////////////////////////////

template<typename ChronoDurationType, long long DesiredTimeSpan>
struct Limiter;

using Limiter60Fps = Limiter<rftl::chrono::nanoseconds, 16666666>; // Monitor
using Limiter30Fps = Limiter<rftl::chrono::nanoseconds, 33333333>; // NTSC
using Limiter24Fps = Limiter<rftl::chrono::nanoseconds, 41666666>; // Film
using Limiter12Fps = Limiter<rftl::chrono::nanoseconds, 83333333>; // Animation

///////////////////////////////////////////////////////////////////////////////

template<typename ChronoDurationType, long long DesiredTimeSpan>
struct Limiter
{
	static constexpr ChronoDurationType kSpanDuration = ChronoDurationType( DesiredTimeSpan );

	void Reset()
	{
		m_SpanStart = RF::time::PerfClock::now();
		m_SpanEnd = RF::time::PerfClock::now();
	}

	void Stall()
	{
		static constexpr RF::time::PerfClock::duration const desiredSpanTime = kSpanDuration;

		time::PerfClock::time_point const naturalSpanEnd = time::PerfClock::now();
		time::PerfClock::duration const naturalSpanTime = naturalSpanEnd - m_SpanStart;
		if( naturalSpanTime < desiredSpanTime )
		{
			time::PerfClock::duration const timeRemaining = desiredSpanTime - naturalSpanTime;
			rftl::this_thread::sleep_for( timeRemaining );
		}
		m_SpanEnd = time::PerfClock::now();
		m_SpanStart = time::PerfClock::now();
	}

	RF::time::PerfClock::time_point m_SpanStart;
	RF::time::PerfClock::time_point m_SpanEnd;
};

///////////////////////////////////////////////////////////////////////////////
}}
