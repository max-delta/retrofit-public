#include "stdafx.h"
#include "clocks.h"


namespace RF { namespace time {
///////////////////////////////////////////////////////////////////////////////

FrameClock::time_point FrameClock::sAccumulatedTime( FrameClock::duration::zero() );
FrameClock::time_point FrameClock::sPreviousAccumulatedTime( FrameClock::duration::zero() );

///////////////////////////////////////////////////////////////////////////////

FrameClock::time_point FrameClock::now()
{
	return sAccumulatedTime;
}



FrameClock::time_point FrameClock::previous()
{
	return sPreviousAccumulatedTime;
}



void FrameClock::add_time( duration const& time )
{
	sPreviousAccumulatedTime = sAccumulatedTime;
	sAccumulatedTime += time;
}

///////////////////////////////////////////////////////////////////////////////
}}
