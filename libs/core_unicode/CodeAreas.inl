#include "stdafx.h"
#include "CodeAreas.h"


namespace RF::unicode {
///////////////////////////////////////////////////////////////////////////////

inline constexpr Plane::Value DeterminePlane( char32_t ch )
{
	// XXXX XXXX XXXn nnnn pppp pppp pppp pppp
	// X:= invalid bits
	// n:= plane 0-16
	// p:= character 0-65,535
	uint32_t const asInt = static_cast<uint32_t>( ch );
	uint32_t const planeVal = ( asInt >> 16u ) & 0b1'1111u;
	Plane::Value const asPlane = static_cast<Plane::Value>( planeVal );
	return asPlane;
}



inline constexpr PrivateUseArea::Value DeterminePrivateUseArea( char32_t ch )
{
	uint32_t const asInt = static_cast<uint32_t>( ch );
	uint32_t const areaVal = asInt >> 8u;
	PrivateUseArea::Value const asArea = static_cast<PrivateUseArea::Value>( areaVal );

	if(
		asArea >= PrivateUseArea::BasicMultilingualPrivate_First &&
		asArea <= PrivateUseArea::BasicMultilingualPrivate_Last )
	{
		return asArea;
	}

	if(
		asArea >= PrivateUseArea::SupplementaryPrivateUseAreaA_First &&
		asArea <= PrivateUseArea::SupplementaryPrivateUseAreaA_Last )
	{
		return asArea;
	}

	if(
		asArea >= PrivateUseArea::SupplementaryPrivateUseAreaB_First &&
		asArea <= PrivateUseArea::SupplementaryPrivateUseAreaB_Last )
	{
		return asArea;
	}

	return PrivateUseArea::Invalid;
}

///////////////////////////////////////////////////////////////////////////////
}
