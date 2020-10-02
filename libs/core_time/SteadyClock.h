#pragma once
#include "rftl/chrono"
#include "rftl/ratio"
#include "rftl/limits"


namespace RF::time {
///////////////////////////////////////////////////////////////////////////////

// Low-precision clock that corresponds to real time
// NOTE: Implemented via QueryPerformanceCounter in MSVC 2019
class SteadyClock : public rftl::chrono::steady_clock
{
private:
	static_assert(
		float( period::num ) / period::den <=
			float( rftl::deci::num ) / rftl::deci::den,
		"SteadyClock has terrible precision" );
	static_assert( is_steady, "SteadyClock isn't steady" );

public:
	static constexpr time_point kLowest = time_point( duration( rep( 0 ) ) );
	static constexpr time_point kMax = time_point( duration( rftl::numeric_limits<rep>::max() ) );
};

///////////////////////////////////////////////////////////////////////////////
}
