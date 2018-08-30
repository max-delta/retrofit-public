#include "stdafx.h"

#include "core_math/BitField.h"


namespace RF { namespace math {
///////////////////////////////////////////////////////////////////////////////

TEST( MathBitField, Empty )
{
	using bf_u64_empty = BitField<uint64_t>;
	static_assert( sizeof( bf_u64_empty ) == 1, "Unexpected size" );
	bf_u64_empty bf = {};
	ASSERT_EQ( reinterpret_cast<uint8_t const*>( bf.Data() )[0], 0x00 );
}



TEST( MathBitField, ZeroSize )
{
	using bf_u64_0_0_0 = BitField<uint64_t, 0, 0, 0>;
	static_assert( sizeof( bf_u64_0_0_0 ) == 1, "Unexpected size" );
	bf_u64_0_0_0 bf = {};
	ASSERT_EQ( reinterpret_cast<uint8_t const*>( bf.Data() )[0], 0x00 );
	bf.WriteAt<0>( 0xffffffffffffffffull );
	ASSERT_EQ( reinterpret_cast<uint8_t const*>( bf.Data() )[0], 0x00 );
	bf.WriteAt<1>( 0xffffffffffffffffull );
	ASSERT_EQ( reinterpret_cast<uint8_t const*>( bf.Data() )[0], 0x00 );
	bf.WriteAt<2>( 0xffffffffffffffffull );
	ASSERT_EQ( reinterpret_cast<uint8_t const*>( bf.Data() )[0], 0x00 );
	ASSERT_EQ( bf.ReadAt<0>(), 0 );
	ASSERT_EQ( bf.ReadAt<1>(), 0 );
	ASSERT_EQ( bf.ReadAt<2>(), 0 );
}



TEST( MathBitField, SizeTruncatedInFirstByte )
{
	using bf_u64_3 = BitField<uint64_t, 3>;
	static_assert( sizeof( bf_u64_3 ) == 1, "Unexpected size" );
	bf_u64_3 bf = {};
	ASSERT_EQ( reinterpret_cast<uint8_t const*>( bf.Data() )[0], 0x00 );
	bf.WriteAt<0>( 0x0d );
	ASSERT_EQ( reinterpret_cast<uint8_t const*>( bf.Data() )[0], 0xa0 );
	ASSERT_EQ( bf.ReadAt<0>(), 0x05 );
}



TEST( MathBitField, SizeTruncatedInSecondByte )
{
	using bf_u64_11 = BitField<uint64_t, 11>;
	static_assert( sizeof( bf_u64_11 ) == 2, "Unexpected size" );
	bf_u64_11 bf = {};
	ASSERT_EQ( reinterpret_cast<uint8_t const*>( bf.Data() )[0], 0x00 );
	ASSERT_EQ( reinterpret_cast<uint8_t const*>( bf.Data() )[1], 0x00 );
	bf.WriteAt<0>( 0x0ffd );
	ASSERT_EQ( reinterpret_cast<uint8_t const*>( bf.Data() )[0], 0xff );
	ASSERT_EQ( reinterpret_cast<uint8_t const*>( bf.Data() )[1], 0xa0 );
	ASSERT_EQ( bf.ReadAt<0>(), 0x07fd );
}



TEST( MathBitField, OffsetTruncationInFirstByte )
{
	using bf_u64_3_3 = BitField<uint64_t, 3, 3>;
	static_assert( sizeof( bf_u64_3_3 ) == 1, "Unexpected size" );
	bf_u64_3_3 bf = {};
	ASSERT_EQ( reinterpret_cast<uint8_t const*>( bf.Data() )[0], 0x00 );
	bf.WriteAt<1>( 0x0d );
	ASSERT_EQ( reinterpret_cast<uint8_t const*>( bf.Data() )[0], 0x14 );
	ASSERT_EQ( bf.ReadAt<1>(), 0x05 );
}



TEST( MathBitField, OffsetTruncationAcrossTwoBytes )
{
	using bf_u64_3_11 = BitField<uint64_t, 3, 11>;
	static_assert( sizeof( bf_u64_3_11 ) == 2, "Unexpected size" );
	bf_u64_3_11 bf = {};
	ASSERT_EQ( reinterpret_cast<uint8_t const*>( bf.Data() )[0], 0x00 );
	ASSERT_EQ( reinterpret_cast<uint8_t const*>( bf.Data() )[1], 0x00 );
	bf.WriteAt<1>( 0x0ffd );
	ASSERT_EQ( reinterpret_cast<uint8_t const*>( bf.Data() )[0], 0x1f );
	ASSERT_EQ( reinterpret_cast<uint8_t const*>( bf.Data() )[1], 0xf4 );
	ASSERT_EQ( bf.ReadAt<1>(), 0x07fd );
}



TEST( MathBitField, OffsetTruncationAcrossThreeBytes )
{
	using bf_u64_3_11 = BitField<uint64_t, 3, 19>;
	static_assert( sizeof( bf_u64_3_11 ) == 3, "Unexpected size" );
	bf_u64_3_11 bf = {};
	ASSERT_EQ( reinterpret_cast<uint8_t const*>( bf.Data() )[0], 0x00 );
	ASSERT_EQ( reinterpret_cast<uint8_t const*>( bf.Data() )[1], 0x00 );
	ASSERT_EQ( reinterpret_cast<uint8_t const*>( bf.Data() )[2], 0x00 );
	bf.WriteAt<1>( 0x0ffffd );
	ASSERT_EQ( reinterpret_cast<uint8_t const*>( bf.Data() )[0], 0x1f );
	ASSERT_EQ( reinterpret_cast<uint8_t const*>( bf.Data() )[1], 0xff );
	ASSERT_EQ( reinterpret_cast<uint8_t const*>( bf.Data() )[2], 0xf4 );
	ASSERT_EQ( bf.ReadAt<1>(), 0x07fffd );
}

///////////////////////////////////////////////////////////////////////////////
}}
