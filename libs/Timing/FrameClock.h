#pragma once
#include "project.h"

#include "core_time/CommonClock.h"

#include "rftl/chrono"
#include "rftl/ratio"


namespace RF::time {
///////////////////////////////////////////////////////////////////////////////

// Frame clock, expected to only increase on frame boundaries
class TIMING_API FrameClock : public CommonClock
{
public:
	static constexpr bool is_steady = false;
	static time_point now();

	static void add_time( duration const& time );
	static void set_time( time_point const& time );

private:
	static time_point sAccumulatedTime;
};

///////////////////////////////////////////////////////////////////////////////
}
