#include "stdafx.h"
#include "clocks.h"


namespace RF { namespace time {
///////////////////////////////////////////////////////////////////////////////

FrameClock::time_point FrameClock::s_AccumulatedTime( FrameClock::duration::zero() );
FrameClock::time_point FrameClock::s_PreviousAccumulatedTime( FrameClock::duration::zero() );

///////////////////////////////////////////////////////////////////////////////

FrameClock::time_point FrameClock::now()
{
	return s_AccumulatedTime;
}



FrameClock::time_point FrameClock::previous()
{
	return s_PreviousAccumulatedTime;
}



void FrameClock::add_time( duration const& time )
{
	s_PreviousAccumulatedTime = s_AccumulatedTime;
	s_AccumulatedTime += time;
}

///////////////////////////////////////////////////////////////////////////////
}}
