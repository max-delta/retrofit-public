#include "stdafx.h"

#include "core_state/StateStream.h"


namespace RF::state {
///////////////////////////////////////////////////////////////////////////////

TEST( StateStream, Basic )
{
	StateStream<int, 4> stream;
	ASSERT_EQ( stream.GetEarliestTime(), ( time::CommonClock::TimePointFromNanos( 0 ) ) );
	ASSERT_EQ( stream.GetLatestTime(), ( time::CommonClock::TimePointFromNanos( 0 ) ) );
	ASSERT_EQ( stream.Read( time::CommonClock::TimePointFromNanos( 0 ) ), int{} );

	stream.Write( time::CommonClock::TimePointFromNanos( 100 ), 3 );
	ASSERT_EQ( stream.GetEarliestTime(), ( time::CommonClock::TimePointFromNanos( 100 ) ) );
	ASSERT_EQ( stream.GetLatestTime(), ( time::CommonClock::TimePointFromNanos( 100 ) ) );
	ASSERT_EQ( stream.Read( time::CommonClock::TimePointFromNanos( 99 ) ), int{} );
	ASSERT_EQ( stream.Read( time::CommonClock::TimePointFromNanos( 100 ) ), 3 );
	ASSERT_EQ( stream.Read( time::CommonClock::TimePointFromNanos( 101 ) ), 3 );

	stream.Write( time::CommonClock::TimePointFromNanos( 300 ), 7 );
	ASSERT_EQ( stream.GetEarliestTime(), ( time::CommonClock::TimePointFromNanos( 100 ) ) );
	ASSERT_EQ( stream.GetLatestTime(), ( time::CommonClock::TimePointFromNanos( 300 ) ) );
	ASSERT_EQ( stream.Read( time::CommonClock::TimePointFromNanos( 99 ) ), int{} );
	ASSERT_EQ( stream.Read( time::CommonClock::TimePointFromNanos( 100 ) ), 3 );
	ASSERT_EQ( stream.Read( time::CommonClock::TimePointFromNanos( 101 ) ), 3 );
	ASSERT_EQ( stream.Read( time::CommonClock::TimePointFromNanos( 299 ) ), 3 );
	ASSERT_EQ( stream.Read( time::CommonClock::TimePointFromNanos( 300 ) ), 7 );
	ASSERT_EQ( stream.Read( time::CommonClock::TimePointFromNanos( 301 ) ), 7 );

	stream.Write( time::CommonClock::TimePointFromNanos( 400 ), 9 );
	ASSERT_EQ( stream.GetEarliestTime(), ( time::CommonClock::TimePointFromNanos( 100 ) ) );
	ASSERT_EQ( stream.GetLatestTime(), ( time::CommonClock::TimePointFromNanos( 400 ) ) );
	ASSERT_EQ( stream.Read( time::CommonClock::TimePointFromNanos( 99 ) ), int{} );
	ASSERT_EQ( stream.Read( time::CommonClock::TimePointFromNanos( 100 ) ), 3 );
	ASSERT_EQ( stream.Read( time::CommonClock::TimePointFromNanos( 101 ) ), 3 );
	ASSERT_EQ( stream.Read( time::CommonClock::TimePointFromNanos( 299 ) ), 3 );
	ASSERT_EQ( stream.Read( time::CommonClock::TimePointFromNanos( 300 ) ), 7 );
	ASSERT_EQ( stream.Read( time::CommonClock::TimePointFromNanos( 301 ) ), 7 );
	ASSERT_EQ( stream.Read( time::CommonClock::TimePointFromNanos( 399 ) ), 7 );
	ASSERT_EQ( stream.Read( time::CommonClock::TimePointFromNanos( 400 ) ), 9 );
	ASSERT_EQ( stream.Read( time::CommonClock::TimePointFromNanos( 401 ) ), 9 );

	// Write to the past with same value truncates to previous change
	stream.Write( time::CommonClock::TimePointFromNanos( 400 ), 7 );
	ASSERT_EQ( stream.GetEarliestTime(), ( time::CommonClock::TimePointFromNanos( 100 ) ) );
	ASSERT_EQ( stream.GetLatestTime(), ( time::CommonClock::TimePointFromNanos( 300 ) ) );
	ASSERT_EQ( stream.Read( time::CommonClock::TimePointFromNanos( 99 ) ), int{} );
	ASSERT_EQ( stream.Read( time::CommonClock::TimePointFromNanos( 100 ) ), 3 );
	ASSERT_EQ( stream.Read( time::CommonClock::TimePointFromNanos( 101 ) ), 3 );
	ASSERT_EQ( stream.Read( time::CommonClock::TimePointFromNanos( 199 ) ), 3 );
	ASSERT_EQ( stream.Read( time::CommonClock::TimePointFromNanos( 200 ) ), 3 );
	ASSERT_EQ( stream.Read( time::CommonClock::TimePointFromNanos( 201 ) ), 3 );
	ASSERT_EQ( stream.Read( time::CommonClock::TimePointFromNanos( 299 ) ), 3 );
	ASSERT_EQ( stream.Read( time::CommonClock::TimePointFromNanos( 300 ) ), 7 );
	ASSERT_EQ( stream.Read( time::CommonClock::TimePointFromNanos( 301 ) ), 7 );
	ASSERT_EQ( stream.Read( time::CommonClock::TimePointFromNanos( 399 ) ), 7 );
	ASSERT_EQ( stream.Read( time::CommonClock::TimePointFromNanos( 400 ) ), 7 );
	ASSERT_EQ( stream.Read( time::CommonClock::TimePointFromNanos( 401 ) ), 7 );

	// Write to the past stomps the future
	stream.Write( time::CommonClock::TimePointFromNanos( 200 ), 5 );
	ASSERT_EQ( stream.GetEarliestTime(), ( time::CommonClock::TimePointFromNanos( 100 ) ) );
	ASSERT_EQ( stream.GetLatestTime(), ( time::CommonClock::TimePointFromNanos( 200 ) ) );
	ASSERT_EQ( stream.Read( time::CommonClock::TimePointFromNanos( 99 ) ), int{} );
	ASSERT_EQ( stream.Read( time::CommonClock::TimePointFromNanos( 100 ) ), 3 );
	ASSERT_EQ( stream.Read( time::CommonClock::TimePointFromNanos( 101 ) ), 3 );
	ASSERT_EQ( stream.Read( time::CommonClock::TimePointFromNanos( 199 ) ), 3 );
	ASSERT_EQ( stream.Read( time::CommonClock::TimePointFromNanos( 200 ) ), 5 );
	ASSERT_EQ( stream.Read( time::CommonClock::TimePointFromNanos( 201 ) ), 5 );
	ASSERT_EQ( stream.Read( time::CommonClock::TimePointFromNanos( 299 ) ), 5 );
	ASSERT_EQ( stream.Read( time::CommonClock::TimePointFromNanos( 300 ) ), 5 );
	ASSERT_EQ( stream.Read( time::CommonClock::TimePointFromNanos( 301 ) ), 5 );
	ASSERT_EQ( stream.Read( time::CommonClock::TimePointFromNanos( 399 ) ), 5 );
	ASSERT_EQ( stream.Read( time::CommonClock::TimePointFromNanos( 400 ) ), 5 );
	ASSERT_EQ( stream.Read( time::CommonClock::TimePointFromNanos( 401 ) ), 5 );

	// Write to current stomps current
	stream.Write( time::CommonClock::TimePointFromNanos( 200 ), 6 );
	ASSERT_EQ( stream.GetEarliestTime(), ( time::CommonClock::TimePointFromNanos( 100 ) ) );
	ASSERT_EQ( stream.GetLatestTime(), ( time::CommonClock::TimePointFromNanos( 200 ) ) );
	ASSERT_EQ( stream.Read( time::CommonClock::TimePointFromNanos( 99 ) ), int{} );
	ASSERT_EQ( stream.Read( time::CommonClock::TimePointFromNanos( 100 ) ), 3 );
	ASSERT_EQ( stream.Read( time::CommonClock::TimePointFromNanos( 101 ) ), 3 );
	ASSERT_EQ( stream.Read( time::CommonClock::TimePointFromNanos( 199 ) ), 3 );
	ASSERT_EQ( stream.Read( time::CommonClock::TimePointFromNanos( 200 ) ), 6 );
	ASSERT_EQ( stream.Read( time::CommonClock::TimePointFromNanos( 201 ) ), 6 );
	ASSERT_EQ( stream.Read( time::CommonClock::TimePointFromNanos( 299 ) ), 6 );
	ASSERT_EQ( stream.Read( time::CommonClock::TimePointFromNanos( 300 ) ), 6 );
	ASSERT_EQ( stream.Read( time::CommonClock::TimePointFromNanos( 301 ) ), 6 );
	ASSERT_EQ( stream.Read( time::CommonClock::TimePointFromNanos( 399 ) ), 6 );
	ASSERT_EQ( stream.Read( time::CommonClock::TimePointFromNanos( 400 ) ), 6 );
	ASSERT_EQ( stream.Read( time::CommonClock::TimePointFromNanos( 401 ) ), 6 );

	// Discard of current returns to the past
	stream.Discard( time::CommonClock::TimePointFromNanos( 200 ) );
	ASSERT_EQ( stream.GetEarliestTime(), ( time::CommonClock::TimePointFromNanos( 100 ) ) );
	ASSERT_EQ( stream.GetLatestTime(), ( time::CommonClock::TimePointFromNanos( 100 ) ) );
	ASSERT_EQ( stream.Read( time::CommonClock::TimePointFromNanos( 99 ) ), int{} );
	ASSERT_EQ( stream.Read( time::CommonClock::TimePointFromNanos( 100 ) ), 3 );
	ASSERT_EQ( stream.Read( time::CommonClock::TimePointFromNanos( 101 ) ), 3 );
	ASSERT_EQ( stream.Read( time::CommonClock::TimePointFromNanos( 199 ) ), 3 );
	ASSERT_EQ( stream.Read( time::CommonClock::TimePointFromNanos( 200 ) ), 3 );
	ASSERT_EQ( stream.Read( time::CommonClock::TimePointFromNanos( 201 ) ), 3 );
	ASSERT_EQ( stream.Read( time::CommonClock::TimePointFromNanos( 299 ) ), 3 );
	ASSERT_EQ( stream.Read( time::CommonClock::TimePointFromNanos( 300 ) ), 3 );
	ASSERT_EQ( stream.Read( time::CommonClock::TimePointFromNanos( 301 ) ), 3 );
	ASSERT_EQ( stream.Read( time::CommonClock::TimePointFromNanos( 399 ) ), 3 );
	ASSERT_EQ( stream.Read( time::CommonClock::TimePointFromNanos( 400 ) ), 3 );
	ASSERT_EQ( stream.Read( time::CommonClock::TimePointFromNanos( 401 ) ), 3 );
}

///////////////////////////////////////////////////////////////////////////////
}
