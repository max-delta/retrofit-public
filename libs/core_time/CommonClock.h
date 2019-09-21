#pragma once
#include "rftl/chrono"


namespace RF { namespace time {
///////////////////////////////////////////////////////////////////////////////

// Common clock declaration to be used by the majority of clocks
class CommonClock
{
public:
	using duration = rftl::chrono::nanoseconds;
	using rep = duration::rep;
	using period = duration::period;
	using time_point = rftl::chrono::time_point<CommonClock>;

public:
	static constexpr time_point TimePointFromNanos( rep nanos )
	{
		return time::CommonClock::time_point( rftl::chrono::nanoseconds( nanos ) );
	}
};

///////////////////////////////////////////////////////////////////////////////
}}
