#pragma once
#include "core/macros.h"

#include "rftl/condition_variable"
#include "rftl/mutex"
#include "rftl/chrono"


namespace RF::thread {
///////////////////////////////////////////////////////////////////////////////

// NOTE: Const semantics are that waiting is treated as non-modifying
class ThreadSignal final
{
	RF_NO_COPY( ThreadSignal );

	//
	// Public methods
public:
	ThreadSignal() = default;
	~ThreadSignal() = default;

	// WARNING: May stop waiting spontaneously, use additional checks to ensure
	//  correct behavior
	// Thread-safe
	void Wait() const
	{
		rftl::unique_lock<rftl::mutex> lock( mMutex );
		mConditionVariable.wait( lock );
	}
	template<typename Duration>
	void WaitFor( Duration duration ) const
	{
		rftl::unique_lock<rftl::mutex> lock( mMutex );
		mConditionVariable.wait_for( lock, duration );
	}
	template<typename TimePoint>
	void WaitUntil( TimePoint timePoint ) const
	{
		rftl::unique_lock<rftl::mutex> lock( mMutex );
		mConditionVariable.wait_until( lock, timePoint );
	}

	// Thread-safe
	void Signal()
	{
		mConditionVariable.notify_all();
	}


	//
	// Private data
private:
	mutable rftl::condition_variable mConditionVariable;
	mutable rftl::mutex mMutex;
};

///////////////////////////////////////////////////////////////////////////////
}
