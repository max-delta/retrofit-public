#include "stdafx.h"

#include "core_math/math_bits.h"


namespace RF { namespace math {
///////////////////////////////////////////////////////////////////////////////

TEST( MathBits, GetAllBitsSet )
{
	static_assert( GetAllBitsSet<uint8_t>() == static_cast<uint8_t>( 0xffu ), "" );
	static_assert( GetAllBitsSet<int8_t>() == static_cast<int8_t>( 0xffu ), "" );
	static_assert( GetAllBitsSet<uint16_t>() == static_cast<uint16_t>( 0xffffu ), "" );
	static_assert( GetAllBitsSet<int16_t>() == static_cast<int16_t>( 0xffffu ), "" );
	static_assert( GetAllBitsSet<uint32_t>() == static_cast<uint32_t>( 0xffffffffu ), "" );
	static_assert( GetAllBitsSet<int32_t>() == static_cast<int32_t>( 0xffffffffu ), "" );
	static_assert( GetAllBitsSet<uint64_t>() == static_cast<uint64_t>( 0xffffffffffffffffu ), "" );
	static_assert( GetAllBitsSet<int64_t>() == static_cast<int64_t>( 0xffffffffffffffffu ), "" );
}



TEST( MathBits, HasOnly1BitSet )
{
	static_assert( HasOnly1BitSet( static_cast<uint8_t>( 0x0 ) ) == false, "" );
	static_assert( HasOnly1BitSet( static_cast<uint8_t>( 0x1 ) ) == true, "" );
	static_assert( HasOnly1BitSet( static_cast<uint8_t>( 0x2 ) ) == true, "" );
	static_assert( HasOnly1BitSet( static_cast<uint8_t>( 0x3 ) ) == false, "" );
	static_assert( HasOnly1BitSet( static_cast<uint8_t>( 0x4 ) ) == true, "" );
	static_assert( HasOnly1BitSet( static_cast<uint8_t>( 0x80 ) ) == true, "" );
	static_assert( HasOnly1BitSet( static_cast<uint8_t>( 0xff ) ) == false, "" );
}



TEST( MathBits, GetOnesIndexOfHighestBit )
{
	static_assert( GetOnesIndexOfHighestBit( 0x0 ) == 0, "" );
	static_assert( GetOnesIndexOfHighestBit( 0x1 ) == 1, "" );
	static_assert( GetOnesIndexOfHighestBit( 0x2 ) == 2, "" );
	static_assert( GetOnesIndexOfHighestBit( 0x3 ) == 2, "" );
	static_assert( GetOnesIndexOfHighestBit( 0x4 ) == 3, "" );
	static_assert( GetOnesIndexOfHighestBit( 0xff ) == 8, "" );
	static_assert( GetOnesIndexOfHighestBit( 0xffff ) == 16, "" );
	static_assert( GetOnesIndexOfHighestBit( 0xffffffff ) == 32, "" );
	static_assert( GetOnesIndexOfHighestBit( 0xffffffffffffffff ) == 64, "" );
}



TEST( MathBits, GetZerosIndexOfHighestBit )
{
	static_assert( GetZerosIndexOfHighestBit( 0x0 ) == -1, "" );
	static_assert( GetZerosIndexOfHighestBit( 0x1 ) == 0, "" );
	static_assert( GetZerosIndexOfHighestBit( 0x2 ) == 1, "" );
	static_assert( GetZerosIndexOfHighestBit( 0x3 ) == 1, "" );
	static_assert( GetZerosIndexOfHighestBit( 0x4 ) == 2, "" );
	static_assert( GetZerosIndexOfHighestBit( 0xff ) == 7, "" );
	static_assert( GetZerosIndexOfHighestBit( 0xffff ) == 15, "" );
	static_assert( GetZerosIndexOfHighestBit( 0xffffffff ) == 31, "" );
	static_assert( GetZerosIndexOfHighestBit( 0xffffffffffffffff ) == 63, "" );
}

///////////////////////////////////////////////////////////////////////////////
}}
