#include "stdafx.h"
#include "GridBitMask.h"

#include "core_math/math_casts.h"


namespace RF::cc::character {
///////////////////////////////////////////////////////////////////////////////

GridBitMask::GridBitMask( bool initialValue )
{
	AssignAllBits( initialValue );
}



bool GridBitMask::FetchBit( character::ElementSlotIndex slotIndex ) const
{
	if( slotIndex.first >= mColumnsByElemLevel.size() )
	{
		RF_DBGFAIL_MSG( "Out of bounds" );
		return false;
	}
	ColumnBits const& bits = mColumnsByElemLevel.at( slotIndex.first );

	if( slotIndex.second >= kBitsPerColumn )
	{
		RF_DBGFAIL_MSG( "Out of bounds" );
		return false;
	}
	ColumnBits const mask = math::integer_cast<ColumnBits>( 1 << slotIndex.second );

	bool const bit = ( bits & mask ) != 0;
	return bit;
}



bool GridBitMask::AssignBit( character::ElementSlotIndex slotIndex, bool newValue )
{
	if( slotIndex.first >= mColumnsByElemLevel.size() )
	{
		RF_DBGFAIL_MSG( "Out of bounds" );
		return false;
	}
	ColumnBits& bits = mColumnsByElemLevel.at( slotIndex.first );

	if( slotIndex.second >= kBitsPerColumn )
	{
		RF_DBGFAIL_MSG( "Out of bounds" );
		return false;
	}
	ColumnBits const mask = math::integer_cast<ColumnBits>( 1 << slotIndex.second );

	bool const bit = ( bits & mask ) != 0;

	if( newValue )
	{
		bits = math::integer_cast<ColumnBits>( bits | mask );
	}
	else
	{
		bits = math::integer_cast<ColumnBits>( bits & ( ~mask ) );
	}

	return bit;
}



void GridBitMask::AssignAllBits( bool newValue )
{
	mColumnsByElemLevel.fill( newValue );
}

///////////////////////////////////////////////////////////////////////////////
}
