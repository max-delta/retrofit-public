#pragma once
#include "project.h"

#include "rftl/chrono"
#include "rftl/ratio"


namespace RF { namespace time {
///////////////////////////////////////////////////////////////////////////////

// Performance clock for measuring with reasonably high-precision, but not
//  rdtsc-level cycle-counting stuff
// NOTE: Implemented via QueryPerformanceCounter in MSVC 2017
typedef rftl::chrono::high_resolution_clock PerfClock;
static_assert(
	float( PerfClock::period::num ) / PerfClock::period::den <=
	float( rftl::micro::num ) / rftl::micro::den,
	"PerfClock has terrible precision" );
static_assert(
	PerfClock::is_steady, "PerfClock isn't steady" );

///////////////////////////////////////////////////////////////////////////////

// Frame clock, expected to only increase on frame boundaries
class TIMING_API FrameClock
{
public:
	typedef rftl::chrono::nanoseconds duration;
	typedef duration::rep rep;
	typedef duration::period period;
	typedef rftl::chrono::time_point<FrameClock> time_point;
	static constexpr bool is_steady = true;
	static time_point now();
	static time_point previous();

	static void add_time( duration const& time );

private:
	static time_point sAccumulatedTime;
	static time_point sPreviousAccumulatedTime;
};

///////////////////////////////////////////////////////////////////////////////
}}
