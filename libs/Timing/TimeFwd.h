#pragma once
#include "rftl/chrono"
#include "rftl/ratio"

namespace RF { namespace time {
///////////////////////////////////////////////////////////////////////////////

class FrameClock;

static constexpr rftl::chrono::nanoseconds k60FpsFrame{ 16666666 }; // Monitor
static constexpr rftl::chrono::nanoseconds k30FpsFrame{ 33333333 }; // NTSC
static constexpr rftl::chrono::nanoseconds k24FpsFrame{ 41666666 }; // Film
static constexpr rftl::chrono::nanoseconds k12FpsFrame{ 83333333 }; // Anime

template<typename ChronoDurationType, long long DesiredTimeSpan>
struct Limiter;

using Limiter60Fps = Limiter<rftl::chrono::nanoseconds, k60FpsFrame.count()>;
using Limiter30Fps = Limiter<rftl::chrono::nanoseconds, k30FpsFrame.count()>;
using Limiter24Fps = Limiter<rftl::chrono::nanoseconds, k24FpsFrame.count()>;
using Limiter12Fps = Limiter<rftl::chrono::nanoseconds, k12FpsFrame.count()>;

///////////////////////////////////////////////////////////////////////////////
}}
