#include "stdafx.h"
#include "Object.h"

#include "core_math/math_clamps.h"

#include "rftl/limits"


namespace RF { namespace gfx {
///////////////////////////////////////////////////////////////////////////////

void Object::Animate()
{
	if( mPaused )
	{
		// Paused, don't animate
		return;
	}

	if( mLooping == false )
	{
		if( mTimeIndex + 1 == mMaxTimeIndex )
		{
			// Auto-pause on last frame
			mPaused = true;
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

///////////////////////////////////////////////////////////////////////////////
}}
