#include "stdafx.h"
#include "Rand.h"

#include "core/macros.h"


namespace RF::math {
///////////////////////////////////////////////////////////////////////////////

uint32_t StableRandLCG( uint32_t previousValue )
{
	// Don't change the algorithm! Code may take dependencies on this behavior

	// Linear congruential generator
	// Numbers from Park, Miller, and Stockmeyer MINSTD
	constexpr uint32_t kModulus = static_cast<uint32_t>( 1ull << 31 );
	constexpr uint32_t kMultiplier = 48271;
	constexpr uint32_t kIncrement = 12345;
	constexpr uint32_t kMask = ~kModulus;
	static_assert( kMask == 0x7FFFFFFF, "Unexpected value" );
	static_assert( kStableRandLCGMaxEntropy == kMask, "Unexpected value" );
	return ( ( previousValue * kMultiplier ) + kIncrement ) & kMask;
}



uint8_t StableRandLCGPercent( uint32_t& lastGeneratorValue )
{
	lastGeneratorValue = StableRandLCG( lastGeneratorValue );
	uint8_t const retVal = lastGeneratorValue % 101;
	RF_ASSERT( retVal <= 100 );
	return retVal;
}



bool StableRandLCGPercentAbove( uint8_t percentThreshold, uint32_t& lastGeneratorValue )
{
	return StableRandLCGPercent( lastGeneratorValue ) > percentThreshold;
}



bool StableRandLCGPercentBelow( uint8_t percentThreshold, uint32_t& lastGeneratorValue )
{
	return StableRandLCGPercent( lastGeneratorValue ) < percentThreshold;
}



uint32_t GetSeedFromHash( uint64_t hash )
{
	uint32_t const upper = static_cast<uint32_t>( (hash & 0xffffffff00000000ull ) >> 32 );
	uint32_t const lower = static_cast<uint32_t>( hash  & 0xffffffffull );
	return upper + lower;
}

///////////////////////////////////////////////////////////////////////////////
}
