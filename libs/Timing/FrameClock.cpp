#include "stdafx.h"
#include "FrameClock.h"


namespace RF::time {
///////////////////////////////////////////////////////////////////////////////

FrameClock::time_point FrameClock::sAccumulatedTime( FrameClock::duration::zero() );

///////////////////////////////////////////////////////////////////////////////

FrameClock::time_point FrameClock::now()
{
	return sAccumulatedTime;
}



void FrameClock::add_time( duration const& time )
{
	sAccumulatedTime += time;
}



void FrameClock::set_time( time_point const& time )
{
	sAccumulatedTime = time;
}

///////////////////////////////////////////////////////////////////////////////
}
