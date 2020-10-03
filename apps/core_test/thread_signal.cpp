#include "stdafx.h"

#include "core_thread/ThreadSignal.h"

#include "rftl/thread"
#include "rftl/functional"
#include "rftl/chrono"


namespace RF::thread {
///////////////////////////////////////////////////////////////////////////////

TEST( ThreadSignal, Basic )
{
	ThreadSignal outgoingSignal;
	ThreadSignal incomingSignal;

	static constexpr auto wait = []( ThreadSignal& out, ThreadSignal& in ) -> void //
	{
		in.Signal();
		out.Wait();
	};

	rftl::chrono::steady_clock::time_point const startTime = rftl::chrono::steady_clock::now();
	{
		rftl::thread thread{ wait, rftl::ref( outgoingSignal ), rftl::ref( incomingSignal ) };
		incomingSignal.Wait();
		outgoingSignal.Signal();
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
