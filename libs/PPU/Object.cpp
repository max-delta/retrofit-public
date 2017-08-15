#include "stdafx.h"
#include "Object.h"

#include "core_math/math_clamps.h"

#include <limits>


namespace RF { namespace gfx {
///////////////////////////////////////////////////////////////////////////////

void Object::Animate()
{
	// User may have put a 0 in, we will count that as a 1
	TimeSlowdownRate const timeSlowdown = math::Max<TimeSlowdownRate>( m_TimeSlowdown, 1 );

	if( m_Paused )
	{
		// Paused, don't animate
		return;
	}

	// If sub-time overcomes the slow-down, then increment real time
	// NOTE: There's potential for tiny short-term timing bugs when slowdown
	//  rate is changed, which can be addressed on user-side by adjusting the
	//  sub-time after change for whatever effect they want to acheive
	m_SubTimeIndex = ( m_SubTimeIndex + 1 ) % timeSlowdown;
	if( m_SubTimeIndex == 0 )
	{
		m_TimeIndex++;
	}
}

///////////////////////////////////////////////////////////////////////////////
}}
