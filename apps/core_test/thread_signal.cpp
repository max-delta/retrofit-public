#include "stdafx.h"

#include "core_thread/ThreadSignal.h"

#include "rftl/atomic"
#include "rftl/thread"
#include "rftl/functional"


namespace RF::thread {
///////////////////////////////////////////////////////////////////////////////

TEST( ThreadSignal, Basic )
{
	ThreadSignal signal;
	rftl::atomic<bool> ready = false;

	static constexpr auto wait = []( rftl::atomic<bool>& ready, ThreadSignal& signal ) -> void //
	{
		ready.store( true, rftl::memory_order::memory_order_release );
		signal.Wait();
	};

	rftl::chrono::steady_clock::time_point const startTime = rftl::chrono::steady_clock::now();
	{
		rftl::thread thread{ wait, rftl::ref( ready ), rftl::ref( signal ) };

		// Once ready, the wait should happen very soon after, barring some
		//  truly absurd scheduling behavior
		// NOTE: In such an edge-case, the signal will be sent and dropped
		//  before the wait has begun, and the wait will be left blocking on
		//  the thread with no further signals coming
		while( ready.load( rftl::memory_order::memory_order_acquire ) == false )
		{
			rftl::this_thread::yield();
		}
		rftl::this_thread::sleep_for( rftl::chrono::milliseconds( 10 ) );
		signal.Signal();

		thread.join();
	}
	rftl::chrono::steady_clock::time_point const endTime = rftl::chrono::steady_clock::now();

	// This entire operation should be very fast when successful. On a failure,
	//  the wait may still spontaneously wake, but this should be infrequent
	//  and thus have a longer elapsed time.
	ASSERT_LE( startTime, endTime );
	rftl::chrono::steady_clock::duration const timeElapsed = endTime - startTime;
	ASSERT_LE( timeElapsed, rftl::chrono::seconds( 1 ) );
}

///////////////////////////////////////////////////////////////////////////////
}
