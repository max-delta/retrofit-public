#include "stdafx.h"
#include "LoggerTime.h"

#include "rftl/atomic"


namespace RF::logging {
///////////////////////////////////////////////////////////////////////////////
namespace details {

static TimingSourceSig*& GetMutableTimingSourceRef()
{
	using namespace rftl::chrono;

	// Default to the high-resolution time since the first point that it was
	//  asked for (presumably close to process start)
	static constexpr logging::TimingSourceSig* defaultTimingSource = []() -> nanoseconds
	{
		using DefaultClock = high_resolution_clock;
		static DefaultClock::time_point earliest = DefaultClock::now();
		DefaultClock::time_point const now = DefaultClock::now();
		return now - earliest;
	};

	static TimingSourceSig* sCallback = defaultTimingSource;
	return sCallback;
}

}
///////////////////////////////////////////////////////////////////////////////

TimingSourceSig* ReplaceTimingSource( TimingSourceSig* callback )
{
	TimingSourceSig*& callbackRef = details::GetMutableTimingSourceRef();
	rftl::atomic_ref<TimingSourceSig*> atomic( callbackRef );
	TimingSourceSig* const retVal = atomic.exchange( callback, rftl::memory_order::relaxed );
	return retVal;
}

///////////////////////////////////////////////////////////////////////////////
namespace details {

rftl::chrono::nanoseconds GetLoggerTime()
{
	TimingSourceSig* const& callbackRef = details::GetMutableTimingSourceRef();
	rftl::atomic_ref<TimingSourceSig* const> atomic( callbackRef );
	TimingSourceSig* const callback = atomic.load( rftl::memory_order::relaxed );
	return callback();
}

}
///////////////////////////////////////////////////////////////////////////////
}
