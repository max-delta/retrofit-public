#include "stdafx.h"

#include "core_math/BitField.h"


namespace RF::math {
///////////////////////////////////////////////////////////////////////////////

TEST( MathBitField, Empty )
{
	using bf_empty = BitField<>;
	static_assert( sizeof( bf_empty ) == 1, "Unexpected size" );
	bf_empty bf = {};
	ASSERT_EQ( reinterpret_cast<uint8_t const*>( bf.Data() )[0], 0x00 );
}



TEST( MathBitField, ZeroSize )
{
	using bf_0_0_0 = BitField<0, 0, 0>;
	static_assert( sizeof( bf_0_0_0 ) == 1, "Unexpected size" );
	bf_0_0_0 bf = {};
	ASSERT_EQ( reinterpret_cast<uint8_t const*>( bf.Data() )[0], 0x00 );
	bf.WriteAt<0>( 0xffffffffffffffffull );
	ASSERT_EQ( reinterpret_cast<uint8_t const*>( bf.Data() )[0], 0x00 );
	bf.WriteAt<1>( 0xffffffffffffffffull );
	ASSERT_EQ( reinterpret_cast<uint8_t const*>( bf.Data() )[0], 0x00 );
	bf.WriteAt<2>( 0xffffffffffffffffull );
	ASSERT_EQ( reinterpret_cast<uint8_t const*>( bf.Data() )[0], 0x00 );
	ASSERT_EQ( ( bf.ReadAt<0, uint64_t>() ), 0 );
	ASSERT_EQ( ( bf.ReadAt<1, uint64_t>() ), 0 );
	ASSERT_EQ( ( bf.ReadAt<2, uint64_t>() ), 0 );
}



TEST( MathBitField, SizeTruncatedInFirstByte )
{
	using bf_3 = BitField<3>;
	static_assert( sizeof( bf_3 ) == 1, "Unexpected size" );
	bf_3 bf = {};
	ASSERT_EQ( reinterpret_cast<uint8_t const*>( bf.Data() )[0], 0x00 );
	bf.WriteAt<0>( 0x0d );
	ASSERT_EQ( reinterpret_cast<uint8_t const*>( bf.Data() )[0], 0xa0 );
	ASSERT_EQ( ( bf.ReadAt<0, uint64_t>() ), 0x05 );
}



TEST( MathBitField, SizeTruncatedInSecondByte )
{
	using bf_11 = BitField<11>;
	static_assert( sizeof( bf_11 ) == 2, "Unexpected size" );
	bf_11 bf = {};
	ASSERT_EQ( reinterpret_cast<uint8_t const*>( bf.Data() )[0], 0x00 );
	ASSERT_EQ( reinterpret_cast<uint8_t const*>( bf.Data() )[1], 0x00 );
	bf.WriteAt<0>( 0x0ffd );
	ASSERT_EQ( reinterpret_cast<uint8_t const*>( bf.Data() )[0], 0xff );
	ASSERT_EQ( reinterpret_cast<uint8_t const*>( bf.Data() )[1], 0xa0 );
	ASSERT_EQ( ( bf.ReadAt<0, uint64_t>() ), 0x07fd );
}



TEST( MathBitField, OffsetTruncationInFirstByte )
{
	using bf_3_3 = BitField<3, 3>;
	static_assert( sizeof( bf_3_3 ) == 1, "Unexpected size" );
	bf_3_3 bf = {};
	ASSERT_EQ( reinterpret_cast<uint8_t const*>( bf.Data() )[0], 0x00 );
	bf.WriteAt<1>( 0x0d );
	ASSERT_EQ( reinterpret_cast<uint8_t const*>( bf.Data() )[0], 0x14 );
	ASSERT_EQ( ( bf.ReadAt<1, uint64_t>() ), 0x05 );
}



TEST( MathBitField, OffsetTruncationAcrossTwoBytes )
{
	using bf_3_11 = BitField<3, 11>;
	static_assert( sizeof( bf_3_11 ) == 2, "Unexpected size" );
	bf_3_11 bf = {};
	ASSERT_EQ( reinterpret_cast<uint8_t const*>( bf.Data() )[0], 0x00 );
	ASSERT_EQ( reinterpret_cast<uint8_t const*>( bf.Data() )[1], 0x00 );
	bf.WriteAt<1>( 0x0ffd );
	ASSERT_EQ( reinterpret_cast<uint8_t const*>( bf.Data() )[0], 0x1f );
	ASSERT_EQ( reinterpret_cast<uint8_t const*>( bf.Data() )[1], 0xf4 );
	ASSERT_EQ( ( bf.ReadAt<1, uint64_t>() ), 0x07fd );
}



TEST( MathBitField, OffsetTruncationAcrossThreeBytes )
{
	using bf_3_11 = BitField<3, 19>;
	static_assert( sizeof( bf_3_11 ) == 3, "Unexpected size" );
	bf_3_11 bf = {};
	ASSERT_EQ( reinterpret_cast<uint8_t const*>( bf.Data() )[0], 0x00 );
	ASSERT_EQ( reinterpret_cast<uint8_t const*>( bf.Data() )[1], 0x00 );
	ASSERT_EQ( reinterpret_cast<uint8_t const*>( bf.Data() )[2], 0x00 );
	bf.WriteAt<1>( 0x0ffffd );
	ASSERT_EQ( reinterpret_cast<uint8_t const*>( bf.Data() )[0], 0x1f );
	ASSERT_EQ( reinterpret_cast<uint8_t const*>( bf.Data() )[1], 0xff );
	ASSERT_EQ( reinterpret_cast<uint8_t const*>( bf.Data() )[2], 0xf4 );
	ASSERT_EQ( ( bf.ReadAt<1, uint64_t>() ), 0x07fffd );
}



TEST( MathBitField, SmallUnsigned )
{
	// Signed/unsigned compilation errors only seem to come into play on
	//  types smaller than uint32_t, so this is mostly just to instantiate the
	//  templates and make sure they compile
	using bf_16 = BitField<16>;
	static_assert( sizeof( bf_16 ) == 2, "Unexpected size" );
	bf_16 bf = {};
	ASSERT_EQ( reinterpret_cast<uint8_t const*>( bf.Data() )[0], 0x00 );
	ASSERT_EQ( reinterpret_cast<uint8_t const*>( bf.Data() )[1], 0x00 );
	bf.WriteAt<0>( 0x1234 );
	ASSERT_EQ( reinterpret_cast<uint8_t const*>( bf.Data() )[0], 0x12 );
	ASSERT_EQ( reinterpret_cast<uint8_t const*>( bf.Data() )[1], 0x34 );
	ASSERT_EQ( ( bf.ReadAt<0, uint16_t>() ), 0x1234 );
}



TEST( MathBitField, LargeSigned )
{
	// Shifting of negative value compilation errors only seem to come into
	//  play on types larger than uint32_t, so this is mostly just to
	//  instantiate the templates and make sure they compile
	using bf_64 = BitField<64>;
	static_assert( sizeof( bf_64 ) == 8, "Unexpected size" );
	bf_64 bf = {};
	ASSERT_EQ( reinterpret_cast<uint8_t const*>( bf.Data() )[0], 0x00 );
	ASSERT_EQ( reinterpret_cast<uint8_t const*>( bf.Data() )[1], 0x00 );
	ASSERT_EQ( reinterpret_cast<uint8_t const*>( bf.Data() )[2], 0x00 );
	ASSERT_EQ( reinterpret_cast<uint8_t const*>( bf.Data() )[3], 0x00 );
	ASSERT_EQ( reinterpret_cast<uint8_t const*>( bf.Data() )[4], 0x00 );
	ASSERT_EQ( reinterpret_cast<uint8_t const*>( bf.Data() )[5], 0x00 );
	ASSERT_EQ( reinterpret_cast<uint8_t const*>( bf.Data() )[6], 0x00 );
	ASSERT_EQ( reinterpret_cast<uint8_t const*>( bf.Data() )[7], 0x00 );
	bf.WriteAt<0>( 0x1234567890abcdef );
	ASSERT_EQ( reinterpret_cast<uint8_t const*>( bf.Data() )[0], 0x12 );
	ASSERT_EQ( reinterpret_cast<uint8_t const*>( bf.Data() )[1], 0x34 );
	ASSERT_EQ( reinterpret_cast<uint8_t const*>( bf.Data() )[2], 0x56 );
	ASSERT_EQ( reinterpret_cast<uint8_t const*>( bf.Data() )[3], 0x78 );
	ASSERT_EQ( reinterpret_cast<uint8_t const*>( bf.Data() )[4], 0x90 );
	ASSERT_EQ( reinterpret_cast<uint8_t const*>( bf.Data() )[5], 0xab );
	ASSERT_EQ( reinterpret_cast<uint8_t const*>( bf.Data() )[6], 0xcd );
	ASSERT_EQ( reinterpret_cast<uint8_t const*>( bf.Data() )[7], 0xef );
	ASSERT_EQ( ( bf.ReadAt<0, int64_t>() ), 0x1234567890abcdef );
}

///////////////////////////////////////////////////////////////////////////////
}
