#include "stdafx.h"

#include "Rollback/InputStream.h"


namespace RF { namespace rollback {
///////////////////////////////////////////////////////////////////////////////

TEST( InputStream, Empty )
{
	static constexpr size_t kNumInputs = 0;
	static constexpr time::CommonClock::time_point kZeroTime = time::CommonClock::TimePointFromNanos( 0 );
	static constexpr time::CommonClock::time_point kTime = time::CommonClock::TimePointFromNanos( 50 );

	InputStream stream;

	ASSERT_EQ( stream.front().mTime, kZeroTime );
	ASSERT_EQ( stream.front().mValue, kInvalidInputValue );
	ASSERT_EQ( stream.back().mTime, kZeroTime );
	ASSERT_EQ( stream.back().mValue, kInvalidInputValue );

	ASSERT_EQ( stream.begin(), stream.cbegin() );
	ASSERT_EQ( stream.end(), stream.cend() );
	ASSERT_EQ( stream.rbegin(), stream.crbegin() );
	ASSERT_EQ( stream.rend(), stream.crend() );

	ASSERT_EQ( stream.begin(), stream.end() );
	ASSERT_EQ( stream.rbegin(), stream.rend() );

	ASSERT_TRUE( stream.empty() );
	ASSERT_EQ( stream.size(), kNumInputs );
	ASSERT_TRUE( stream.max_size() > 0 );

	ASSERT_EQ( stream.upper_bound( kTime ), stream.end() );
	ASSERT_EQ( stream.lower_bound( kTime ), stream.end() );
}



TEST( InputStream, Single )
{
	static constexpr size_t kNumInputs = 1;
	static constexpr InputValue kVal = 7;
	static constexpr time::CommonClock::time_point kTime = time::CommonClock::TimePointFromNanos( 50 );

	InputStream stream;

	stream.emplace_back( kTime, kVal );

	ASSERT_EQ( stream.front().mTime, kTime );
	ASSERT_EQ( stream.front().mValue, kVal );
	ASSERT_EQ( stream.back().mTime, kTime );
	ASSERT_EQ( stream.back().mValue, kVal );

	ASSERT_EQ( stream.begin(), stream.cbegin() );
	ASSERT_EQ( stream.end(), stream.cend() );
	ASSERT_EQ( stream.rbegin(), stream.crbegin() );
	ASSERT_EQ( stream.rend(), stream.crend() );

	ASSERT_NE( stream.begin(), stream.end() );
	ASSERT_EQ( stream.begin()->mTime, stream.front().mTime );
	ASSERT_EQ( stream.begin()->mValue, stream.front().mValue );

	ASSERT_NE( stream.rbegin(), stream.rend() );
	ASSERT_EQ( &*stream.rbegin(), &*( stream.begin() + kNumInputs - 1 ) );

	ASSERT_FALSE( stream.empty() );
	ASSERT_EQ( stream.size(), kNumInputs );
	ASSERT_TRUE( stream.max_size() > 0 );

	ASSERT_EQ( stream.upper_bound( kTime ), stream.end() );
	ASSERT_EQ( stream.lower_bound( kTime ), stream.begin() );
}



TEST( InputStream, TripleFrame )
{
	static constexpr size_t kNumInputs = 6;
	static constexpr time::CommonClock::time_point kTimes[kNumInputs] = {
		time::CommonClock::TimePointFromNanos( 30 ),
		time::CommonClock::TimePointFromNanos( 30 ),
		time::CommonClock::TimePointFromNanos( 40 ),
		time::CommonClock::TimePointFromNanos( 40 ),
		time::CommonClock::TimePointFromNanos( 50 ),
		time::CommonClock::TimePointFromNanos( 50 )
	};
	static constexpr InputValue kVals[kNumInputs] = { 1, 2, 3, 4, 5, 6 };
	static constexpr time::CommonClock::time_point kZeroTime = time::CommonClock::TimePointFromNanos( 0 );
	static constexpr time::CommonClock::time_point kForward = time::CommonClock::TimePointFromNanos( 60 );

	InputStream stream;

	// 6 frames
	for( size_t i = 0; i < kNumInputs; i++ )
	{
		stream.emplace_back( kTimes[i], kVals[i] );
	}
	{
		ASSERT_EQ( stream.front().mTime, kTimes[0] );
		ASSERT_EQ( stream.front().mValue, kVals[0] );
		ASSERT_EQ( stream.back().mTime, kTimes[kNumInputs - 1] );
		ASSERT_EQ( stream.back().mValue, kVals[kNumInputs - 1] );

		ASSERT_EQ( stream.begin(), stream.cbegin() );
		ASSERT_EQ( stream.end(), stream.cend() );
		ASSERT_EQ( stream.rbegin(), stream.crbegin() );
		ASSERT_EQ( stream.rend(), stream.crend() );

		ASSERT_NE( stream.begin(), stream.end() );
		ASSERT_EQ( stream.begin()->mTime, stream.front().mTime );
		ASSERT_EQ( stream.begin()->mValue, stream.front().mValue );

		ASSERT_NE( stream.rbegin(), stream.rend() );
		ASSERT_EQ( &*stream.rbegin(), &*( stream.begin() + static_cast<ptrdiff_t>( stream.size() - 1u ) ) );

		ASSERT_FALSE( stream.empty() );
		ASSERT_EQ( stream.size(), kNumInputs );
		ASSERT_TRUE( stream.max_size() > 0 );

		ASSERT_EQ( stream.upper_bound( kTimes[0] ), stream.begin() + 2 );
		ASSERT_EQ( stream.upper_bound( kTimes[1] ), stream.begin() + 2 );
		ASSERT_EQ( stream.upper_bound( kTimes[2] ), stream.begin() + 4 );
		ASSERT_EQ( stream.upper_bound( kTimes[3] ), stream.begin() + 4 );
		ASSERT_EQ( stream.upper_bound( kTimes[4] ), stream.end() );
		ASSERT_EQ( stream.upper_bound( kTimes[5] ), stream.end() );
		ASSERT_EQ( stream.upper_bound( kForward ), stream.end() );

		ASSERT_EQ( stream.lower_bound( kTimes[0] ), stream.begin() );
		ASSERT_EQ( stream.lower_bound( kTimes[1] ), stream.begin() );
		ASSERT_EQ( stream.lower_bound( kTimes[2] ), stream.begin() + 2 );
		ASSERT_EQ( stream.lower_bound( kTimes[3] ), stream.begin() + 2 );
		ASSERT_EQ( stream.lower_bound( kTimes[4] ), stream.begin() + 4 );
		ASSERT_EQ( stream.lower_bound( kTimes[5] ), stream.begin() + 4 );
		ASSERT_EQ( stream.lower_bound( kForward ), stream.end() );
	}

	// Increase write head
	stream.increase_write_head( kForward );
	{
		ASSERT_EQ( stream.front().mTime, kTimes[0] );
		ASSERT_EQ( stream.front().mValue, kVals[0] );
		ASSERT_EQ( stream.back().mTime, kForward ); // Changed
		ASSERT_EQ( stream.back().mValue, kInvalidInputValue ); // Changed

		ASSERT_EQ( stream.begin(), stream.cbegin() );
		ASSERT_EQ( stream.end(), stream.cend() );
		ASSERT_EQ( stream.rbegin(), stream.crbegin() );
		ASSERT_EQ( stream.rend(), stream.crend() );

		ASSERT_NE( stream.begin(), stream.end() );
		ASSERT_EQ( stream.begin()->mTime, stream.front().mTime );
		ASSERT_EQ( stream.begin()->mValue, stream.front().mValue );

		ASSERT_NE( stream.rbegin(), stream.rend() );
		ASSERT_EQ( &*stream.rbegin(), &*( stream.begin() + static_cast<ptrdiff_t>( stream.size() - 1u ) ) );

		ASSERT_FALSE( stream.empty() );
		ASSERT_EQ( stream.size(), kNumInputs );
		ASSERT_TRUE( stream.max_size() > 0 );

		ASSERT_EQ( stream.upper_bound( kTimes[0] ), stream.begin() + 2 );
		ASSERT_EQ( stream.upper_bound( kTimes[1] ), stream.begin() + 2 );
		ASSERT_EQ( stream.upper_bound( kTimes[2] ), stream.begin() + 4 );
		ASSERT_EQ( stream.upper_bound( kTimes[3] ), stream.begin() + 4 );
		ASSERT_EQ( stream.upper_bound( kTimes[4] ), stream.end() );
		ASSERT_EQ( stream.upper_bound( kTimes[5] ), stream.end() );
		ASSERT_EQ( stream.upper_bound( kForward ), stream.end() );

		ASSERT_EQ( stream.lower_bound( kTimes[0] ), stream.begin() );
		ASSERT_EQ( stream.lower_bound( kTimes[1] ), stream.begin() );
		ASSERT_EQ( stream.lower_bound( kTimes[2] ), stream.begin() + 2 );
		ASSERT_EQ( stream.lower_bound( kTimes[3] ), stream.begin() + 2 );
		ASSERT_EQ( stream.lower_bound( kTimes[4] ), stream.begin() + 4 );
		ASSERT_EQ( stream.lower_bound( kTimes[5] ), stream.begin() + 4 );
		ASSERT_EQ( stream.lower_bound( kForward ), stream.end() );
	}

	// Increase read head
	stream.increase_read_head( kTimes[2] );
	{
		ASSERT_EQ( stream.front().mTime, kTimes[2] ); // Changed
		ASSERT_EQ( stream.front().mValue, kVals[2] ); // Changed
		ASSERT_EQ( stream.back().mTime, kForward );
		ASSERT_EQ( stream.back().mValue, kInvalidInputValue );

		ASSERT_EQ( stream.begin(), stream.cbegin() );
		ASSERT_EQ( stream.end(), stream.cend() );
		ASSERT_EQ( stream.rbegin(), stream.crbegin() );
		ASSERT_EQ( stream.rend(), stream.crend() );

		ASSERT_NE( stream.begin(), stream.end() );
		ASSERT_EQ( stream.begin()->mTime, stream.front().mTime );
		ASSERT_EQ( stream.begin()->mValue, stream.front().mValue );

		ASSERT_NE( stream.rbegin(), stream.rend() );
		ASSERT_EQ( &*stream.rbegin(), &*( stream.begin() + static_cast<ptrdiff_t>( stream.size() - 1u ) ) );

		ASSERT_FALSE( stream.empty() );
		ASSERT_EQ( stream.size(), kNumInputs - 2 ); // Changed
		ASSERT_TRUE( stream.max_size() > 0 );

		ASSERT_EQ( stream.upper_bound( kTimes[0] ), stream.begin() ); // Changed
		ASSERT_EQ( stream.upper_bound( kTimes[1] ), stream.begin() ); // Changed
		ASSERT_EQ( stream.upper_bound( kTimes[2] ), stream.begin() + 2 ); // Changed
		ASSERT_EQ( stream.upper_bound( kTimes[3] ), stream.begin() + 2 ); // Changed
		ASSERT_EQ( stream.upper_bound( kTimes[4] ), stream.end() );
		ASSERT_EQ( stream.upper_bound( kTimes[5] ), stream.end() );
		ASSERT_EQ( stream.upper_bound( kForward ), stream.end() );

		ASSERT_EQ( stream.lower_bound( kTimes[0] ), stream.begin() );
		ASSERT_EQ( stream.lower_bound( kTimes[1] ), stream.begin() );
		ASSERT_EQ( stream.lower_bound( kTimes[2] ), stream.begin() ); // Changed
		ASSERT_EQ( stream.lower_bound( kTimes[3] ), stream.begin() ); // Changed
		ASSERT_EQ( stream.lower_bound( kTimes[4] ), stream.begin() + 2 ); // Changed
		ASSERT_EQ( stream.lower_bound( kTimes[5] ), stream.begin() + 2 ); // Changed
		ASSERT_EQ( stream.lower_bound( kForward ), stream.end() );
	}

	// Rewind
	stream.rewind( kTimes[4] );
	{
		ASSERT_EQ( stream.front().mTime, kTimes[2] );
		ASSERT_EQ( stream.front().mValue, kVals[2] );
		ASSERT_EQ( stream.back().mTime, kTimes[4] ); // Changed
		ASSERT_EQ( stream.back().mValue, kInvalidInputValue ); // Changed

		ASSERT_EQ( stream.begin(), stream.cbegin() );
		ASSERT_EQ( stream.end(), stream.cend() );
		ASSERT_EQ( stream.rbegin(), stream.crbegin() );
		ASSERT_EQ( stream.rend(), stream.crend() );

		ASSERT_NE( stream.begin(), stream.end() );
		ASSERT_EQ( stream.begin()->mTime, stream.front().mTime );
		ASSERT_EQ( stream.begin()->mValue, stream.front().mValue );

		ASSERT_NE( stream.rbegin(), stream.rend() );
		ASSERT_EQ( &*stream.rbegin(), &*( stream.begin() + static_cast<ptrdiff_t>( stream.size() - 1u ) ) );

		ASSERT_FALSE( stream.empty() );
		ASSERT_EQ( stream.size(), kNumInputs - 4 ); // Changed
		ASSERT_TRUE( stream.max_size() > 0 );

		ASSERT_EQ( stream.upper_bound( kTimes[0] ), stream.begin() );
		ASSERT_EQ( stream.upper_bound( kTimes[1] ), stream.begin() );
		ASSERT_EQ( stream.upper_bound( kTimes[2] ), stream.end() ); // Changed
		ASSERT_EQ( stream.upper_bound( kTimes[3] ), stream.end() ); // Changed
		ASSERT_EQ( stream.upper_bound( kTimes[4] ), stream.end() );
		ASSERT_EQ( stream.upper_bound( kTimes[5] ), stream.end() );
		ASSERT_EQ( stream.upper_bound( kForward ), stream.end() );

		ASSERT_EQ( stream.lower_bound( kTimes[0] ), stream.begin() );
		ASSERT_EQ( stream.lower_bound( kTimes[1] ), stream.begin() );
		ASSERT_EQ( stream.lower_bound( kTimes[2] ), stream.begin() );
		ASSERT_EQ( stream.lower_bound( kTimes[3] ), stream.begin() );
		ASSERT_EQ( stream.lower_bound( kTimes[4] ), stream.end() ); // Changed
		ASSERT_EQ( stream.lower_bound( kTimes[5] ), stream.end() ); // Changed
		ASSERT_EQ( stream.lower_bound( kForward ), stream.end() );
	}

	// Reset
	stream.reset();
	{
		ASSERT_EQ( stream.front().mTime, kZeroTime ); // Changed
		ASSERT_EQ( stream.front().mValue, kInvalidInputValue ); // Changed
		ASSERT_EQ( stream.back().mTime, kZeroTime ); // Changed
		ASSERT_EQ( stream.back().mValue, kInvalidInputValue );

		ASSERT_EQ( stream.begin(), stream.cbegin() );
		ASSERT_EQ( stream.end(), stream.cend() );
		ASSERT_EQ( stream.rbegin(), stream.crbegin() );
		ASSERT_EQ( stream.rend(), stream.crend() );

		ASSERT_EQ( stream.begin(), stream.end() ); // Changed
		ASSERT_EQ( stream.rbegin(), stream.rend() ); // Changed

		ASSERT_TRUE( stream.empty() ); // Changed
		ASSERT_EQ( stream.size(), 0 ); // Changed
		ASSERT_TRUE( stream.max_size() > 0 );

		ASSERT_EQ( stream.upper_bound( kTimes[0] ), stream.end() ); // Changed
		ASSERT_EQ( stream.upper_bound( kTimes[1] ), stream.end() ); // Changed
		ASSERT_EQ( stream.upper_bound( kTimes[2] ), stream.end() );
		ASSERT_EQ( stream.upper_bound( kTimes[3] ), stream.end() );
		ASSERT_EQ( stream.upper_bound( kTimes[4] ), stream.end() );
		ASSERT_EQ( stream.upper_bound( kTimes[5] ), stream.end() );
		ASSERT_EQ( stream.upper_bound( kForward ), stream.end() );

		ASSERT_EQ( stream.lower_bound( kTimes[0] ), stream.end() ); // Changed
		ASSERT_EQ( stream.lower_bound( kTimes[1] ), stream.end() ); // Changed
		ASSERT_EQ( stream.lower_bound( kTimes[2] ), stream.end() ); // Changed
		ASSERT_EQ( stream.lower_bound( kTimes[3] ), stream.end() ); // Changed
		ASSERT_EQ( stream.lower_bound( kTimes[4] ), stream.end() );
		ASSERT_EQ( stream.lower_bound( kTimes[5] ), stream.end() );
		ASSERT_EQ( stream.lower_bound( kForward ), stream.end() );
	}
}

///////////////////////////////////////////////////////////////////////////////
}}
