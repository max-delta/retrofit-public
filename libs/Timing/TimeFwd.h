#pragma once
#include "rftl/chrono"
#include "rftl/ratio"

namespace RF { namespace time {
///////////////////////////////////////////////////////////////////////////////

class FrameClock;

static constexpr rftl::chrono::nanoseconds k60FpsFrame{ 16'666'666 }; // Monitor
static constexpr rftl::chrono::nanoseconds k30FpsFrame{ 33'333'333 }; // NTSC
static constexpr rftl::chrono::nanoseconds k24FpsFrame{ 41'666'666 }; // Film
static constexpr rftl::chrono::nanoseconds k12FpsFrame{ 83'333'333 }; // Anime
static constexpr rftl::chrono::milliseconds k2FpsFrame{ 500 }; // Debug
static constexpr rftl::chrono::seconds k1FpsFrame{ 1 }; // Debug

template<typename ChronoDurationType, long long DesiredTimeSpan>
struct Limiter;

using Limiter60Fps = Limiter<decltype( k60FpsFrame ), k60FpsFrame.count()>;
using Limiter30Fps = Limiter<decltype( k30FpsFrame ), k30FpsFrame.count()>;
using Limiter24Fps = Limiter<decltype( k24FpsFrame ), k24FpsFrame.count()>;
using Limiter12Fps = Limiter<decltype( k12FpsFrame ), k12FpsFrame.count()>;
using Limiter2Fps = Limiter<decltype( k2FpsFrame ), k2FpsFrame.count()>;
using Limiter1Fps = Limiter<decltype( k1FpsFrame ), k1FpsFrame.count()>;

///////////////////////////////////////////////////////////////////////////////
}}
