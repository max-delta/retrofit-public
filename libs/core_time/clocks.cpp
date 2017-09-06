#include "stdafx.h"
#include "clocks.h"


namespace RF { namespace time {
///////////////////////////////////////////////////////////////////////////////

FrameClock::time_point FrameClock::s_AccumulatedTime( FrameClock::duration::zero() );

///////////////////////////////////////////////////////////////////////////////

FrameClock::time_point FrameClock::now()
{
	return s_AccumulatedTime;
}



void FrameClock::add_time( duration const& time )
{
	s_AccumulatedTime += time;
}

///////////////////////////////////////////////////////////////////////////////
}}
