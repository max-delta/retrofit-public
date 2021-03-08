#include "stdafx.h"

#include "core_math/BitField.h"


RF_CLANG_IGNORE( "-Wundefined-reinterpret-cast" );

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



TEST( MathBitField, IEEE754Binary32Nonsense )
{
	static_assert( rftl::numeric_limits<float>::is_iec559, "This test assumes standard floats" );
	using bf_1_7_24 = BitField<1, 8, 23>;
	static_assert( sizeof( bf_1_7_24 ) == 4, "Unexpected size" );
	static_assert( sizeof( float ) == 4, "Unexpected size" );
	static_assert( sizeof( uint32_t ) == 4, "Unexpected size" );
	auto convertToSoft = []( float hardFloat ) -> bf_1_7_24 //
	{
		// HACK: This is probably wildly unsafe and hardware-dependent
		uint32_t const softUnswapped = *reinterpret_cast<uint32_t const*>( &hardFloat );
		uint32_t const softSwapped = math::FromPlatformToBigEndian( softUnswapped );
		return *reinterpret_cast<bf_1_7_24 const*>( &softSwapped );
	};
	{
		bf_1_7_24 const softFloat = convertToSoft( 0.f );
		ASSERT_EQ( ( softFloat.ReadAt<0, uint32_t>() ), 0 ); // ... * -1^<SIGN> * ...
		ASSERT_EQ( ( softFloat.ReadAt<1, uint32_t>() ), 0 ); // ... * 2^(<EXP>-127) * ...
		ASSERT_EQ( ( softFloat.ReadAt<2, uint32_t>() ), 0b00000000000000000000000 ); // ... * (1+FRAC) * ...
	}
	{
		bf_1_7_24 const softFloat = convertToSoft( -0.f );
		ASSERT_EQ( ( softFloat.ReadAt<0, uint32_t>() ), 1 ); // ... * -1^<SIGN> * ...
		ASSERT_EQ( ( softFloat.ReadAt<1, uint32_t>() ), 0 ); // ... * 2^(<EXP>-127) * ...
		ASSERT_EQ( ( softFloat.ReadAt<2, uint32_t>() ), 0b00000000000000000000000 ); // ... * (1+FRAC) * ...
	}
	{
		bf_1_7_24 const softFloat = convertToSoft( -2.f );
		ASSERT_EQ( ( softFloat.ReadAt<0, uint32_t>() ), 1 ); // ... * -1^<SIGN> * ...
		ASSERT_EQ( ( softFloat.ReadAt<1, uint32_t>() ), 128 ); // ... * 2^(<EXP>-127) * ...
		ASSERT_EQ( ( softFloat.ReadAt<2, uint32_t>() ), 0b00000000000000000000000 ); // ... * (1+FRAC) * ...
	}
	{
		bf_1_7_24 const softFloat = convertToSoft( -3.f );
		ASSERT_EQ( ( softFloat.ReadAt<0, uint32_t>() ), 1 ); // ... * -1^<SIGN> * ...
		ASSERT_EQ( ( softFloat.ReadAt<1, uint32_t>() ), 128 ); // ... * 2^(<EXP>-127) * ...
		ASSERT_EQ( ( softFloat.ReadAt<2, uint32_t>() ), 0b10000000000000000000000 ); // ... * (1+FRAC) * ...
	}
	{
		bf_1_7_24 const softFloat = convertToSoft( rftl::numeric_limits<float>::denorm_min() );
		ASSERT_EQ( ( softFloat.ReadAt<0, uint32_t>() ), 0 ); // ... * -1^<SIGN> * ...
		ASSERT_EQ( ( softFloat.ReadAt<1, uint32_t>() ), 0 ); // ... * 2^(<EXP>-127) * ...
		ASSERT_EQ( ( softFloat.ReadAt<2, uint32_t>() ), 0b00000000000000000000001 ); // ... * (1+FRAC) * ...
	}
	{
		bf_1_7_24 const softFloat = convertToSoft( rftl::numeric_limits<float>::infinity() );
		ASSERT_EQ( ( softFloat.ReadAt<0, uint32_t>() ), 0 ); // ... * -1^<SIGN> * ...
		ASSERT_EQ( ( softFloat.ReadAt<1, uint32_t>() ), 255 ); // ... * 2^(<EXP>-127) * ...
		ASSERT_EQ( ( softFloat.ReadAt<2, uint32_t>() ), 0b00000000000000000000000 ); // ... * (1+FRAC) * ...
	}
	{
		bf_1_7_24 const softFloat = convertToSoft( rftl::numeric_limits<float>::quiet_NaN() );
		ASSERT_EQ( ( softFloat.ReadAt<0, uint32_t>() ), 0 ); // ... * -1^<SIGN> * ...
		ASSERT_EQ( ( softFloat.ReadAt<1, uint32_t>() ), 255 ); // ... * 2^(<EXP>-127) * ...
		ASSERT_EQ( ( softFloat.ReadAt<2, uint32_t>() ), 0b10000000000000000000000 ); // ... * (1+FRAC) * ...
	}
	{
		bf_1_7_24 const softFloat = convertToSoft( rftl::numeric_limits<float>::signaling_NaN() );
		ASSERT_EQ( ( softFloat.ReadAt<0, uint32_t>() ), 0 ); // ... * -1^<SIGN> * ...
		ASSERT_EQ( ( softFloat.ReadAt<1, uint32_t>() ), 255 ); // ... * 2^(<EXP>-127) * ...
		switch( compiler::kCompiler )
		{
			case compiler::Compiler::MSVC:
				ASSERT_EQ( ( softFloat.ReadAt<2, uint32_t>() ), 0b10000000000000000000001 ); // ... * (1+FRAC) * ...
				break;
			case compiler::Compiler::Clang:
			{
				uint32_t const frac = softFloat.ReadAt<2, uint32_t>(); // ... * (1+FRAC) * ...
				if constexpr( compiler::kArchitecture == compiler::Architecture::x86_32 )
				{
					// Clang seems to have some bizarre behaviors here that
					//  results in inconsistent literals in the assembly
					ASSERT_TRUE(
						frac == 0b00000000000000000000001 ||
						frac == 0b10000000000000000000001 );
				}
				else
				{
					ASSERT_EQ( frac, 0b00000000000000000000001 );
				}
				break;
			}
			case compiler::Compiler::Invalid:
			default:
				// Unimplemented
				ASSERT_TRUE( false );
				break;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
}
