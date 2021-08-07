#pragma once
#include "rftl/chrono"
#include "rftl/limits"


namespace RF::time {
///////////////////////////////////////////////////////////////////////////////

// Common clock declaration to be used by the majority of clocks
class CommonClock
{
public:
	using duration = rftl::chrono::nanoseconds;
	using rep = duration::rep;
	using period = duration::period;
	using time_point = rftl::chrono::time_point<CommonClock>;

	static constexpr time_point kLowest = time_point( duration( rep( 0 ) ) );
	static constexpr time_point kMax = time_point( duration( rftl::numeric_limits<rep>::max() ) );

public:
	static constexpr time_point TimePointFromNanos( rep nanos )
	{
		return time::CommonClock::time_point( rftl::chrono::nanoseconds( nanos ) );
	}
};

///////////////////////////////////////////////////////////////////////////////
}
