#include "stdafx.h"

#include "core_math/Color3f.h"
#include "core_math/Color3u8.h"
#include "core_math/Color4a5.h"
#include "core_math/Color4u8.h"

#include "rftl/extension/array_convert.h"


namespace RF::math {
///////////////////////////////////////////////////////////////////////////////

TEST( Color, BasicConversions )
{
#define RF_ARR( TYPE ) { TYPE::kRed, TYPE::kGreen, TYPE::kBlue }
	rftl::array const kArr3f = RF_ARR( Color3f );
	rftl::array const kArr3u8 = RF_ARR( Color3u8 );
	rftl::array const kArr4a5 = RF_ARR( Color4a5 );
	rftl::array const kArr4u8 = RF_ARR( Color4u8 );
#undef RF_ARR

	{
		ASSERT_EQ( kArr3u8, rftl::array_convert<Color3u8>( kArr3u8 ) );
		ASSERT_EQ( kArr3u8, rftl::array_convert<Color3u8>( kArr3f ) );
		ASSERT_EQ( kArr3u8, rftl::array_convert<Color3u8>( kArr4a5 ) );
		ASSERT_EQ( kArr3u8, rftl::array_convert<Color3u8>( kArr4u8 ) );
	}
	{
		ASSERT_EQ( kArr3f, rftl::array_convert<Color3f>( kArr3u8 ) );
		ASSERT_EQ( kArr3f, rftl::array_convert<Color3f>( kArr3f ) );
		ASSERT_EQ( kArr3f, rftl::array_convert<Color3f>( kArr4a5 ) );
		ASSERT_EQ( kArr3f, rftl::array_convert<Color3f>( kArr4u8 ) );
	}
	{
		ASSERT_EQ( kArr4a5, rftl::array_convert<Color4a5>( kArr3u8 ) );
		ASSERT_EQ( kArr4a5, rftl::array_convert<Color4a5>( kArr3f ) );
		ASSERT_EQ( kArr4a5, rftl::array_convert<Color4a5>( kArr4a5 ) );
		ASSERT_EQ( kArr4a5, rftl::array_convert<Color4a5>( kArr4u8 ) );
	}
	{
		ASSERT_EQ( kArr4u8, rftl::array_convert<Color4u8>( kArr3u8 ) );
		ASSERT_EQ( kArr4u8, rftl::array_convert<Color4u8>( kArr3f ) );
		ASSERT_EQ( kArr4u8, rftl::array_convert<Color4u8>( kArr4a5 ) );
		ASSERT_EQ( kArr4u8, rftl::array_convert<Color4u8>( kArr4u8 ) );
	}
}

///////////////////////////////////////////////////////////////////////////////
}
