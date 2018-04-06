#include "stdafx.h"

#include "core_math/math_casts.h"


namespace RF { namespace math {
///////////////////////////////////////////////////////////////////////////////

TEST(MathCasts, IntegerTruncast_Unsigned)
{
	constexpr uint64_t huge = std::numeric_limits<uint64_t>::max();
	uint64_t const u64 = integer_truncast<uint64_t>( huge );
	uint64_t const u32 = integer_truncast<uint32_t>( huge );
	uint64_t const u16 = integer_truncast<uint16_t>( huge );
	uint64_t const u8 = integer_truncast<uint8_t>( huge );
	ASSERT_EQ( u64, std::numeric_limits<uint64_t>::max() );
	ASSERT_EQ( u32, std::numeric_limits<uint32_t>::max() );
	ASSERT_EQ( u16, std::numeric_limits<uint16_t>::max() );
	ASSERT_EQ( u8, std::numeric_limits<uint8_t>::max() );
}



TEST(MathCasts, IntegerTruncast_Signed)
{
	static_assert( std::numeric_limits<int64_t>::lowest() < 0, "" );

	constexpr int64_t huge = std::numeric_limits<int64_t>::lowest();
	int64_t const s64 = integer_truncast<int64_t>( huge );
	int64_t const s32 = integer_truncast<int32_t>( huge );
	int64_t const s16 = integer_truncast<int16_t>( huge );
	int64_t const s8 = integer_truncast<int8_t>( huge );
	ASSERT_EQ( s64, std::numeric_limits<int64_t>::lowest() );
	ASSERT_EQ( s32, std::numeric_limits<int32_t>::lowest() );
	ASSERT_EQ( s16, std::numeric_limits<int16_t>::lowest() );
	ASSERT_EQ( s8, std::numeric_limits<int8_t>::lowest() );
}



TEST(MathCasts, IntegerTruncast_CrossSigned)
{
	static_assert( std::numeric_limits<int64_t>::lowest() < 0, "" );
	static_assert( std::numeric_limits<uint64_t>::lowest() == 0, "" );

	constexpr int64_t huge = std::numeric_limits<int64_t>::lowest();
	uint64_t const u64 = integer_truncast<uint64_t>( huge );
	uint64_t const u32 = integer_truncast<uint32_t>( huge );
	uint64_t const u16 = integer_truncast<uint16_t>( huge );
	uint64_t const u8 = integer_truncast<uint8_t>( huge );
	ASSERT_EQ( u64, std::numeric_limits<uint64_t>::lowest() );
	ASSERT_EQ( u32, std::numeric_limits<uint32_t>::lowest() );
	ASSERT_EQ( u16, std::numeric_limits<uint16_t>::lowest() );
	ASSERT_EQ( u8, std::numeric_limits<uint8_t>::lowest() );
}



TEST( MathCasts, IntegerTruncast_CrossUnsigned )
{
	static_assert( std::numeric_limits<int64_t>::lowest() < 0, "" );
	static_assert( std::numeric_limits<uint64_t>::lowest() == 0, "" );

	constexpr uint64_t zeroed = std::numeric_limits<uint64_t>::lowest();
	int64_t const s64 = integer_truncast<int64_t>( zeroed );
	int64_t const s32 = integer_truncast<int32_t>( zeroed );
	int64_t const s16 = integer_truncast<int16_t>( zeroed );
	int64_t const s8 = integer_truncast<int8_t>( zeroed );
	ASSERT_EQ( s64, zeroed );
	ASSERT_EQ( s32, zeroed );
	ASSERT_EQ( s16, zeroed );
	ASSERT_EQ( s8, zeroed );
}



TEST(MathCasts, IntegerTruncast_Float)
{
	{
		constexpr float source = -1.f;
		uint8_t const val = integer_truncast<uint8_t>(source);
		ASSERT_EQ( val, 0 );
	}
	{
		constexpr float source = 0.f;
		uint8_t const val = integer_truncast<uint8_t>(source);
		ASSERT_EQ( val, 0 );
	}
	{
		constexpr float source = 0.1f;
		uint8_t const val = integer_truncast<uint8_t>(source);
		ASSERT_EQ( val, 0 );
	}
	{
		constexpr float source = 0.9f;
		uint8_t const val = integer_truncast<uint8_t>(source);
		ASSERT_EQ( val, 1 );
	}
	{
		constexpr float source = 1.1f;
		uint8_t const val = integer_truncast<uint8_t>(source);
		ASSERT_EQ( val, 1 );
	}
	{
		constexpr float source = std::numeric_limits<float>::max();
		uint8_t const val = integer_truncast<uint8_t>(source);
		ASSERT_EQ( val, std::numeric_limits<uint8_t>::max() );
	}
	{
		constexpr float source = std::numeric_limits<float>::infinity();
		uint8_t const val = integer_truncast<uint8_t>(source);
		ASSERT_EQ( val, std::numeric_limits<uint8_t>::max() );
	}
	{
		// Hmm...
		static_assert( std::numeric_limits<uint8_t>::quiet_NaN() == 0, "" );
		constexpr float source = std::numeric_limits<float>::quiet_NaN();
		uint8_t const val = integer_truncast<uint8_t>(source);
		ASSERT_EQ( val, std::numeric_limits<uint8_t>::quiet_NaN() );
	}
	{
		// Uh... sure, I guess?
		static_assert( std::numeric_limits<uint8_t>::signaling_NaN() == 0, "" );
		constexpr float source = std::numeric_limits<float>::signaling_NaN();
		uint8_t const val = integer_truncast<uint8_t>(source);
		ASSERT_EQ( val, std::numeric_limits<uint8_t>::signaling_NaN() );
	}
}

///////////////////////////////////////////////////////////////////////////////
}}
