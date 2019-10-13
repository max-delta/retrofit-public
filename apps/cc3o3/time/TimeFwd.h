#pragma once
#include "project.h"

#include "Timing/TimeFwd.h"


namespace RF { namespace cc { namespace time {
///////////////////////////////////////////////////////////////////////////////

using namespace RF::time;

// NOTE: These should be kept in sync unless performing some local debugging
using FrameLimiter = Limiter60Fps;
static constexpr rftl::chrono::nanoseconds kGraphicalFrameDuration = k60FpsFrame;
static constexpr rftl::chrono::nanoseconds kSimulationFrameDuration = kGraphicalFrameDuration;

///////////////////////////////////////////////////////////////////////////////
}}}
