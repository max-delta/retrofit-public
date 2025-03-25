#include "stdafx.h"

#include "core_math/math_bits.h"


namespace RF::math {
///////////////////////////////////////////////////////////////////////////////

TEST( MathBits, GetAllBitsSet )
{
	static_assert( GetAllBitsSet<uint8_t>() == static_cast<uint8_t>( 0xffu ) );
	static_assert( GetAllBitsSet<int8_t>() == static_cast<int8_t>( 0xffu ) );
	static_assert( GetAllBitsSet<uint16_t>() == static_cast<uint16_t>( 0xffffu ) );
	static_assert( GetAllBitsSet<int16_t>() == static_cast<int16_t>( 0xffffu ) );
	static_assert( GetAllBitsSet<uint32_t>() == static_cast<uint32_t>( 0xffffffffu ) );
	static_assert( GetAllBitsSet<int32_t>() == static_cast<int32_t>( 0xffffffffu ) );
	static_assert( GetAllBitsSet<uint64_t>() == static_cast<uint64_t>( 0xffffffffffffffffu ) );
	static_assert( GetAllBitsSet<int64_t>() == static_cast<int64_t>( 0xffffffffffffffffu ) );
}



TEST( MathBits, GetBitWidth )
{
	static_assert( GetBitWidth<uint8_t>() == 8 );
	static_assert( GetBitWidth<int8_t>() == 8 );
	static_assert( GetBitWidth<uint16_t>() == 16 );
	static_assert( GetBitWidth<int16_t>() == 16 );
	static_assert( GetBitWidth<uint32_t>() == 32 );
	static_assert( GetBitWidth<int32_t>() == 32 );
	static_assert( GetBitWidth<uint64_t>() == 64 );
	static_assert( GetBitWidth<int64_t>() == 64 );
}



TEST( MathBits, HasOnly1BitSet )
{
	static_assert( HasOnly1BitSet( static_cast<uint8_t>( 0x0 ) ) == false );
	static_assert( HasOnly1BitSet( static_cast<uint8_t>( 0x1 ) ) == true );
	static_assert( HasOnly1BitSet( static_cast<uint8_t>( 0x2 ) ) == true );
	static_assert( HasOnly1BitSet( static_cast<uint8_t>( 0x3 ) ) == false );
	static_assert( HasOnly1BitSet( static_cast<uint8_t>( 0x4 ) ) == true );
	static_assert( HasOnly1BitSet( static_cast<uint8_t>( 0x80 ) ) == true );
	static_assert( HasOnly1BitSet( static_cast<uint8_t>( 0xff ) ) == false );
}



TEST( MathBits, GetOnesIndexOfHighestBit )
{
	static_assert( GetOnesIndexOfHighestBit( 0x0 ) == 0 );
	static_assert( GetOnesIndexOfHighestBit( 0x1 ) == 1 );
	static_assert( GetOnesIndexOfHighestBit( 0x2 ) == 2 );
	static_assert( GetOnesIndexOfHighestBit( 0x3 ) == 2 );
	static_assert( GetOnesIndexOfHighestBit( 0x4 ) == 3 );
	static_assert( GetOnesIndexOfHighestBit( 0xff ) == 8 );
	static_assert( GetOnesIndexOfHighestBit( 0xffff ) == 16 );
	static_assert( GetOnesIndexOfHighestBit( 0xffffffff ) == 32 );
	static_assert( GetOnesIndexOfHighestBit( 0xffffffffffffffff ) == 64 );

	static_assert( GetOnesIndexOfHighestBit( static_cast<int8_t>( -1ll ) ) == 8 );
	static_assert( GetOnesIndexOfHighestBit( static_cast<int16_t>( -1ll ) ) == 16 );
	static_assert( GetOnesIndexOfHighestBit( static_cast<int32_t>( -1ll ) ) == 32 );
	static_assert( GetOnesIndexOfHighestBit( static_cast<int64_t>( -1ll ) ) == 64 );
}



TEST( MathBits, GetZerosIndexOfHighestBit )
{
	static_assert( GetZerosIndexOfHighestBit( 0x0 ) == static_cast<size_t>( -1 ) );
	static_assert( GetZerosIndexOfHighestBit( 0x1 ) == 0 );
	static_assert( GetZerosIndexOfHighestBit( 0x2 ) == 1 );
	static_assert( GetZerosIndexOfHighestBit( 0x3 ) == 1 );
	static_assert( GetZerosIndexOfHighestBit( 0x4 ) == 2 );
	static_assert( GetZerosIndexOfHighestBit( 0xff ) == 7 );
	static_assert( GetZerosIndexOfHighestBit( 0xffff ) == 15 );
	static_assert( GetZerosIndexOfHighestBit( 0xffffffff ) == 31 );
	static_assert( GetZerosIndexOfHighestBit( 0xffffffffffffffff ) == 63 );

	static_assert( GetZerosIndexOfHighestBit( static_cast<int8_t>( -1ll ) ) == 7 );
	static_assert( GetZerosIndexOfHighestBit( static_cast<int16_t>( -1ll ) ) == 15 );
	static_assert( GetZerosIndexOfHighestBit( static_cast<int32_t>( -1ll ) ) == 31 );
	static_assert( GetZerosIndexOfHighestBit( static_cast<int64_t>( -1ll ) ) == 63 );
}



TEST( MathBits, IsBitSet )
{
	// Every 7th bit is set
	uint8_t const array[] = { 1, 2, 4, 8, 16 };
	constexpr size_t numBits = sizeof( array ) * 8;

	for( size_t i = 0; i < numBits; i++ )
	{
		bool const isSet = IsBitSet( array, i );
		if( i != 0 && ( i % 7 == 0 ) )
		{
			ASSERT_TRUE( isSet );
		}
		else
		{
			ASSERT_FALSE( isSet );
		}
	}
}



TEST( MathBits, SetBit )
{
	uint8_t array[] = { 0, 0, 0, 0, 0 };
	constexpr size_t numBits = sizeof( array ) * 8;

	// Every 7th bit is set
	for( size_t i = 7; i < numBits; i += 7 )
	{
		SetBit( array, i );
	}

	ASSERT_EQ( array[0], 1 );
	ASSERT_EQ( array[1], 2 );
	ASSERT_EQ( array[2], 4 );
	ASSERT_EQ( array[3], 8 );
	ASSERT_EQ( array[4], 16 );
}



TEST( MathBits, ClearBit )
{
	uint8_t array[] = { 0xff, 0xff, 0xff, 0xff, 0xff };
	constexpr size_t numBits = sizeof( array ) * 8;

	// Every 7th bit is cleared
	for( size_t i = 7; i < numBits; i += 7 )
	{
		ClearBit( array, i );
	}

	ASSERT_EQ( array[0], static_cast<uint8_t>( ~1 ) );
	ASSERT_EQ( array[1], static_cast<uint8_t>( ~2 ) );
	ASSERT_EQ( array[2], static_cast<uint8_t>( ~4 ) );
	ASSERT_EQ( array[3], static_cast<uint8_t>( ~8 ) );
	ASSERT_EQ( array[4], static_cast<uint8_t>( ~16 ) );
}

///////////////////////////////////////////////////////////////////////////////
}
