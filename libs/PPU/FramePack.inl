#pragma once
#include "FramePack.h"


namespace RF { namespace gfx {
///////////////////////////////////////////////////////////////////////////////

template<size_t TIMESLOTS>
inline FramePack<TIMESLOTS>::FramePack()
	: FramePackBase( kMaxTimeSlots )
{
	RF_ASSERT( mMaxTimeSlots == kMaxTimeSlots );

	for( uint8_t& timeSlotSustain : mTimeSlotSustains )
	{
		timeSlotSustain = uint8_t{};
	}

	for( TimeSlot& timeSlot : mTimeSlots )
	{
		timeSlot = TimeSlot{};
	}
}

///////////////////////////////////////////////////////////////////////////////
}}
