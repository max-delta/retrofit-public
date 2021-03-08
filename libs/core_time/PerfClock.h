#pragma once
#include "rftl/chrono"
#include "rftl/ratio"


namespace RF::time {
///////////////////////////////////////////////////////////////////////////////

// Performance clock for measuring with reasonably high-precision, but not
//  rdtsc-level cycle-counting stuff
// NOTE: Implemented via QueryPerformanceCounter in MSVC 2017
using PerfClock = rftl::chrono::high_resolution_clock;
static_assert(
	float( PerfClock::period::num ) / PerfClock::period::den <=
	float( rftl::micro::num ) / rftl::micro::den,
	"PerfClock has terrible precision" );
static_assert(
	PerfClock::is_steady, "PerfClock isn't steady" );

///////////////////////////////////////////////////////////////////////////////
}
