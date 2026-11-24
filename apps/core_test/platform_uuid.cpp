#include "stdafx.h"

#include "core_platform/uuid.h"


namespace RF::platform {
///////////////////////////////////////////////////////////////////////////////

TEST( Uuid, Invalid )
{
	Uuid const defaultInvalid{};
	Uuid const explicitInvalid{ Uuid::kInvalid };

	ASSERT_FALSE( defaultInvalid.IsValid() );
	ASSERT_FALSE( explicitInvalid.IsValid() );
}



TEST( Uuid, CompareEarliestOctet )
{
	constexpr Uuid::OctetSequence kOne = { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	constexpr Uuid::OctetSequence kTwo = { 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	Uuid const one{ kOne };
	Uuid const two{ kTwo };
	ASSERT_TRUE( one.IsValid() );
	ASSERT_TRUE( two.IsValid() );

	ASSERT_TRUE( one == one );
	ASSERT_TRUE( one != two );
	ASSERT_TRUE( one < two );
	ASSERT_TRUE( one <= one );
	ASSERT_TRUE( one <= two );
	ASSERT_TRUE( two > one );
	ASSERT_TRUE( two >= one );
	ASSERT_TRUE( two >= two );

	ASSERT_FALSE( one == two );
	ASSERT_FALSE( one != one );
	ASSERT_FALSE( two < one );
	ASSERT_FALSE( two <= one );
	ASSERT_FALSE( one > two );
	ASSERT_FALSE( one >= two );
}



TEST( Uuid, CompareLatestOctet )
{
	constexpr Uuid::OctetSequence kLow = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 };
	constexpr Uuid::OctetSequence kHigh = { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	Uuid const low{ kLow };
	Uuid const high{ kHigh };
	ASSERT_TRUE( low.IsValid() );
	ASSERT_TRUE( high.IsValid() );

	ASSERT_TRUE( low == low );
	ASSERT_TRUE( low != high );
	ASSERT_TRUE( low < high );
	ASSERT_TRUE( low <= low );
	ASSERT_TRUE( low <= high );
	ASSERT_TRUE( high > low );
	ASSERT_TRUE( high >= low );
	ASSERT_TRUE( high >= high );

	ASSERT_FALSE( low == high );
	ASSERT_FALSE( low != low );
	ASSERT_FALSE( high < low );
	ASSERT_FALSE( high <= low );
	ASSERT_FALSE( low > high );
	ASSERT_FALSE( low >= high );
}



TEST( Uuid, Clone )
{
	constexpr Uuid::OctetSequence kSeq = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 };
	Uuid const seq1{ kSeq };
	ASSERT_TRUE( seq1.IsValid() );
	ASSERT_TRUE( seq1 == seq1 );

	Uuid const seq2{ seq1 };
	ASSERT_TRUE( seq2.IsValid() );
	ASSERT_TRUE( seq2 == seq1 );
	ASSERT_TRUE( seq2 == seq2 );

	Uuid const seq3{ seq2.GetSequence() };
	ASSERT_TRUE( seq3.IsValid() );
	ASSERT_TRUE( seq3 == seq1 );
	ASSERT_TRUE( seq3 == seq2 );
	ASSERT_TRUE( seq3 == seq3 );
}



TEST( Uuid, Format )
{
	ASSERT_EQ(
		Uuid( { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } ).GetDebugString(),
		"{00000000-0000-0000-0000-000000000000}(v0,0)" );
	ASSERT_EQ(
		Uuid( { 0, 0, 0, 0, 0, 0, 0x10, 0, 0x80, 0, 0, 0, 0, 0, 0, 0 } ).GetDebugString(),
		"{00000000-0000-1000-8000-000000000000}(v1!#,4)" );
	ASSERT_EQ(
		Uuid( { 0, 0, 0, 0, 0, 0, 0x20, 0, 0x80, 0, 0, 0, 0, 0, 0, 0 } ).GetDebugString(),
		"{00000000-0000-2000-8000-000000000000}(v2!,4)" );
	ASSERT_EQ(
		Uuid( { 0, 0, 0, 0, 0, 0, 0x30, 0, 0x80, 0, 0, 0, 0, 0, 0, 0 } ).GetDebugString(),
		"{00000000-0000-3000-8000-000000000000}(v3!,4)" );
	ASSERT_EQ(
		Uuid( { 0, 0, 0, 0, 0, 0, 0x40, 0, 0x80, 0, 0, 0, 0, 0, 0, 0 } ).GetDebugString(),
		"{00000000-0000-4000-8000-000000000000}(v4,4)" );
	ASSERT_EQ(
		Uuid( { 0, 0, 0, 0, 0, 0, 0x50, 0, 0x80, 0, 0, 0, 0, 0, 0, 0 } ).GetDebugString(),
		"{00000000-0000-5000-8000-000000000000}(v5!,4)" );
}

///////////////////////////////////////////////////////////////////////////////
}
