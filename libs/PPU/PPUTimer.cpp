#include "stdafx.h"
#include "PPUTimer.h"


namespace RF::gfx {
///////////////////////////////////////////////////////////////////////////////

void PPUTimer::Animate( bool looping, bool paused )
{
	if( paused )
	{
		// Paused, don't animate
		return;
	}

	if( looping == false )
	{
		if( mTimeIndex + 1 == mMaxTimeIndex )
		{
			// Auto-pause on last frame
			paused = true;
			return;
		}
	}

	// User may have put a 0 in, we will count that as a 1
	TimeSlowdownRate const timeSlowdown = math::Max<TimeSlowdownRate>( mTimeSlowdown, 1 );

	// If sub-time overcomes the slow-down, then increment real time
	// NOTE: There's potential for tiny short-term timing bugs when slowdown
	//  rate is changed, which can be addressed on user-side by adjusting the
	//  sub-time after change for whatever effect they want to acheive
	mSubTimeIndex = math::integer_cast<uint8_t>( ( mSubTimeIndex + 1 ) % timeSlowdown );
	if( mSubTimeIndex == 0 )
	{
		mTimeIndex++;
	}

	if( mTimeIndex == mMaxTimeIndex )
	{
		// Force rollover
		mTimeIndex = 0;
	}
}



bool PPUTimer::IsFullZero() const
{
	return mTimeIndex == 0 && mSubTimeIndex == 0;
}

///////////////////////////////////////////////////////////////////////////////
}
