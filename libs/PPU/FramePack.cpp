#include "stdafx.h"
#include "FramePack.h"

#include "core_math/math_clamps.h"
#include "core_math/math_casts.h"


namespace RF { namespace gfx {
///////////////////////////////////////////////////////////////////////////////

uint8_t const* FramePackBase::GetTimeSlotSustains() const
{
	uint8_t const* const thisPtr = reinterpret_cast<uint8_t const*>( this );
	size_t const offsetOfDerived = offsetof( FramePackBase, m_MaxTimeSlots ) + 1;
	size_t const offsetOfTimeSlotSustains = offsetOfDerived + 0;
	RF_ASSERT( m_MaxTimeSlots == FramePack_4096::k_MaxTimeSlots ? offsetOfTimeSlotSustains == offsetof( FramePack_4096, m_TimeSlotSustains ) : true );
	RF_ASSERT( m_MaxTimeSlots == FramePack_1024::k_MaxTimeSlots ? offsetOfTimeSlotSustains == offsetof( FramePack_1024, m_TimeSlotSustains ) : true );
	RF_ASSERT( m_MaxTimeSlots == FramePack_512::k_MaxTimeSlots ? offsetOfTimeSlotSustains == offsetof( FramePack_512, m_TimeSlotSustains ) : true );
	RF_ASSERT( m_MaxTimeSlots == FramePack_256::k_MaxTimeSlots ? offsetOfTimeSlotSustains == offsetof( FramePack_256, m_TimeSlotSustains ) : true );
	return reinterpret_cast<uint8_t const*>( thisPtr + offsetOfTimeSlotSustains );
}



uint8_t* FramePackBase::GetMutableTimeSlotSustains()
{
	return const_cast<uint8_t*>( GetTimeSlotSustains() );
}



FramePackBase::TimeSlot const* FramePackBase::GetTimeSlots() const
{
	uint8_t const* const thisPtr = reinterpret_cast<uint8_t const*>( this );
	size_t const offsetOfDerived = offsetof( FramePackBase, m_MaxTimeSlots ) + 1;
	size_t const offsetOfTimeSlotSustains = offsetOfDerived + 0;
	size_t const sizeOfTimeSlotSustains = sizeof( uint8_t ) * m_MaxTimeSlots;
	size_t const sizeOfTimeSlotSustainsPadding = ( 8 - ( sizeOfTimeSlotSustains % 8 ) ) % 8;
	size_t const offsetOfTimeSlots = offsetOfTimeSlotSustains + sizeOfTimeSlotSustains + sizeOfTimeSlotSustainsPadding;
	RF_ASSERT( m_MaxTimeSlots == FramePack_4096::k_MaxTimeSlots ? offsetOfTimeSlots == offsetof( FramePack_4096, m_TimeSlots ) : true );
	RF_ASSERT( m_MaxTimeSlots == FramePack_1024::k_MaxTimeSlots ? offsetOfTimeSlots == offsetof( FramePack_1024, m_TimeSlots ) : true );
	RF_ASSERT( m_MaxTimeSlots == FramePack_512::k_MaxTimeSlots ? offsetOfTimeSlots == offsetof( FramePack_512, m_TimeSlots ) : true );
	RF_ASSERT( m_MaxTimeSlots == FramePack_256::k_MaxTimeSlots ? offsetOfTimeSlots == offsetof( FramePack_256, m_TimeSlots ) : true );
	return reinterpret_cast<TimeSlot const*>( thisPtr + offsetOfTimeSlots );
}



FramePackBase::TimeSlot* FramePackBase::GetMutableTimeSlots()
{
	return const_cast<TimeSlot*>( GetTimeSlots() );
}



uint8_t FramePackBase::CalculateTimeSlotFromTimeIndex( uint8_t timeIndex ) const
{
	RF_ASSERT( m_NumTimeSlots <= m_MaxTimeSlots );

	uint8_t const* const timeSlotSustains = GetTimeSlotSustains();

	uint8_t rollingTimeIndex = 0;
	for( uint8_t i = 0; i < m_NumTimeSlots; i++ )
	{
		// User may have put a 0 in, we will count that as a 1
		uint8_t const timeSlotSustain = math::Max<uint8_t>( timeSlotSustains[i], 1 );

		rollingTimeIndex += timeSlotSustain;
		if( timeIndex < rollingTimeIndex )
		{
			// Accumulated enough time, time index falls here
			return i;
		}
	}
	uint8_t const maxTimeIndex = rollingTimeIndex;

	// Couldn't accumulate enough before hitting end of frame-pack, this means
	//  it's rolled over. We'll just calc the wrap, and re-run once to find
	//  where it lies.
	return CalculateTimeSlotFromTimeIndex( math::integer_cast<uint8_t>( timeIndex % maxTimeIndex ) );
}



uint8_t FramePackBase::CalculateFirstTimeIndexOfTimeSlot( uint8_t timeSlot ) const
{
	RF_ASSERT( m_NumTimeSlots <= m_MaxTimeSlots );
	RF_ASSERT( timeSlot < m_NumTimeSlots );

	uint8_t const* const timeSlotSustains = GetTimeSlotSustains();

	uint8_t rollingTimeIndex = 0;
	for( uint8_t i = 0; i < m_NumTimeSlots; i++ )
	{
		if( i == timeSlot )
		{
			// Accumulated enough time, time index falls here
			return rollingTimeIndex;
		}

		// User may have put a 0 in, we will count that as a 1
		uint8_t const timeSlotSustain = math::Max<uint8_t>( timeSlotSustains[i], 1 );

		rollingTimeIndex += timeSlotSustain;
	}

	RF_DBGFAIL();
	return 0;
}



uint8_t FramePackBase::CalculateTimeIndexBoundary() const
{
	RF_ASSERT( m_NumTimeSlots <= m_MaxTimeSlots );

	uint8_t const* const timeSlotSustains = GetTimeSlotSustains();

	uint64_t accumulator = 0;
	for( uint8_t i = 0; i < m_NumTimeSlots; i++ )
	{
		// User may have put a 0 in, we will count that as a 1
		uint8_t const timeSlotSustain = math::Max<uint8_t>( timeSlotSustains[i], 1 );

		accumulator += timeSlotSustain;
	}

	RF_ASSERT_MSG( accumulator < std::numeric_limits<uint8_t>::max(), "Animation length exceeds max time value" );
	return math::integer_cast<uint8_t>( accumulator );
}



FramePackBase::FramePackBase( uint8_t maxTimeSlots )
	: m_MaxTimeSlots( maxTimeSlots )
{
	//
}

///////////////////////////////////////////////////////////////////////////////
}}
