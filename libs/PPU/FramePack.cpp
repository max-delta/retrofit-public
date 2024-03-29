#include "stdafx.h"
#include "FramePack.h"

#include "core_math/math_clamps.h"
#include "core_math/math_casts.h"

#include "rftl/cstring"


namespace RF::gfx::ppu {
///////////////////////////////////////////////////////////////////////////////

uint8_t const* FramePackBase::GetTimeSlotSustains() const
{
	uint8_t const* const thisPtr = reinterpret_cast<uint8_t const*>( this );
	size_t const offsetOfDerived = offsetof( FramePackBase, mMaxTimeSlots ) + 1;
	size_t const offsetOfTimeSlotSustains = offsetOfDerived + 0;

	RF_CLANG_PUSH();
	RF_CLANG_IGNORE( "-Winvalid-offsetof" );
	RF_ASSERT( mMaxTimeSlots == FramePack_4096::kMaxTimeSlots ? offsetOfTimeSlotSustains == offsetof( FramePack_4096, mTimeSlotSustains ) : true );
	RF_ASSERT( mMaxTimeSlots == FramePack_1024::kMaxTimeSlots ? offsetOfTimeSlotSustains == offsetof( FramePack_1024, mTimeSlotSustains ) : true );
	RF_ASSERT( mMaxTimeSlots == FramePack_512::kMaxTimeSlots ? offsetOfTimeSlotSustains == offsetof( FramePack_512, mTimeSlotSustains ) : true );
	RF_ASSERT( mMaxTimeSlots == FramePack_256::kMaxTimeSlots ? offsetOfTimeSlotSustains == offsetof( FramePack_256, mTimeSlotSustains ) : true );
	RF_CLANG_POP();

	return reinterpret_cast<uint8_t const*>( thisPtr + offsetOfTimeSlotSustains );
}



uint8_t* FramePackBase::GetMutableTimeSlotSustains()
{
	return const_cast<uint8_t*>( GetTimeSlotSustains() );
}



FramePackBase::TimeSlot const* FramePackBase::GetTimeSlots() const
{
	uint8_t const* const thisPtr = reinterpret_cast<uint8_t const*>( this );
	size_t const offsetOfDerived = offsetof( FramePackBase, mMaxTimeSlots ) + 1;
	size_t const offsetOfTimeSlotSustains = offsetOfDerived + 0;
	size_t const sizeOfTimeSlotSustains = sizeof( uint8_t ) * mMaxTimeSlots;
	size_t const sizeOfTimeSlotSustainsPadding = ( 8 - ( sizeOfTimeSlotSustains % 8 ) ) % 8;
	size_t const offsetOfTimeSlots = offsetOfTimeSlotSustains + sizeOfTimeSlotSustains + sizeOfTimeSlotSustainsPadding;

	RF_CLANG_PUSH();
	RF_CLANG_IGNORE( "-Winvalid-offsetof" );
	RF_ASSERT( mMaxTimeSlots == FramePack_4096::kMaxTimeSlots ? offsetOfTimeSlots == offsetof( FramePack_4096, mTimeSlots ) : true );
	RF_ASSERT( mMaxTimeSlots == FramePack_1024::kMaxTimeSlots ? offsetOfTimeSlots == offsetof( FramePack_1024, mTimeSlots ) : true );
	RF_ASSERT( mMaxTimeSlots == FramePack_512::kMaxTimeSlots ? offsetOfTimeSlots == offsetof( FramePack_512, mTimeSlots ) : true );
	RF_ASSERT( mMaxTimeSlots == FramePack_256::kMaxTimeSlots ? offsetOfTimeSlots == offsetof( FramePack_256, mTimeSlots ) : true );
	RF_CLANG_POP();

	return reinterpret_cast<TimeSlot const*>( thisPtr + offsetOfTimeSlots );
}



FramePackBase::TimeSlot* FramePackBase::GetMutableTimeSlots()
{
	return const_cast<TimeSlot*>( GetTimeSlots() );
}



uint8_t FramePackBase::CalculateTimeSlotFromTimeIndex( uint8_t timeIndex ) const
{
	RF_ASSERT( mNumTimeSlots <= mMaxTimeSlots );

	uint8_t const* const timeSlotSustains = GetTimeSlotSustains();

	uint8_t rollingTimeIndex = 0;
	for( uint8_t i = 0; i < mNumTimeSlots; i++ )
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
	RF_ASSERT( mNumTimeSlots <= mMaxTimeSlots );
	RF_ASSERT( timeSlot < mNumTimeSlots );

	uint8_t const* const timeSlotSustains = GetTimeSlotSustains();

	uint8_t rollingTimeIndex = 0;
	for( uint8_t i = 0; i < mNumTimeSlots; i++ )
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
	RF_ASSERT( mNumTimeSlots <= mMaxTimeSlots );

	uint8_t const* const timeSlotSustains = GetTimeSlotSustains();

	uint64_t accumulator = 0;
	for( uint8_t i = 0; i < mNumTimeSlots; i++ )
	{
		// User may have put a 0 in, we will count that as a 1
		uint8_t const timeSlotSustain = math::Max<uint8_t>( timeSlotSustains[i], 1 );

		accumulator += timeSlotSustain;
	}

	RF_ASSERT_MSG( accumulator < rftl::numeric_limits<uint8_t>::max(), "Animation length exceeds max time value" );
	return math::integer_cast<uint8_t>( accumulator );
}



void FramePackBase::CopyBaseValuesFrom( FramePackBase const& rhs )
{
	// NOTE: The memcpy will blow over const variables, so we need to make
	//  sure to save and restore them appropriately
	using MaxSlotsType = rftl::remove_const<decltype( rhs.mMaxTimeSlots )>::type;
	MaxSlotsType const savedMaxSlots = rhs.mMaxTimeSlots;
	{
		uint8_t const* const readHead = reinterpret_cast<uint8_t const*>( &rhs );
		uint8_t* const writeHead = reinterpret_cast<uint8_t*>( this );
		rftl::memcpy( writeHead, readHead, sizeof( FramePackBase ) );
	}
	*const_cast<MaxSlotsType*>( &this->mMaxTimeSlots ) = savedMaxSlots;
}



FramePackBase::FramePackBase( uint8_t maxTimeSlots )
	: mMaxTimeSlots( maxTimeSlots )
{
	//
}

///////////////////////////////////////////////////////////////////////////////
}
