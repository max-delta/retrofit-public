#pragma once
#include "project.h"

#include "core_time/CommonClock.h"

#include "rftl/chrono"
#include "rftl/ratio"


namespace RF { namespace time {
///////////////////////////////////////////////////////////////////////////////

// Frame clock, expected to only increase on frame boundaries
class TIMING_API FrameClock : public CommonClock
{
public:
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
