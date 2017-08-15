#pragma once
#include "FramePack.h"


namespace RF { namespace gfx {
///////////////////////////////////////////////////////////////////////////////

template<size_t TIMESLOTS>
inline FramePack<TIMESLOTS>::FramePack()
	: FramePackBase( k_MaxTimeSlots )
{
	RF_ASSERT( m_MaxTimeSlots == k_MaxTimeSlots );

	for( uint8_t& timeSlotSustain : m_TimeSlotSustains )
	{
		timeSlotSustain = uint8_t{};
	}

	for( TimeSlot& timeSlot : m_TimeSlots )
	{
		timeSlot = TimeSlot{};
	}
}

///////////////////////////////////////////////////////////////////////////////
}}
