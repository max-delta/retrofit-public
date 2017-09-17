#include "stdafx.h"

#include "core_math/Vector2.h"


namespace RF { namespace math {
///////////////////////////////////////////////////////////////////////////////

TEST( Vector2, Default_CommonTypes )
{
	ASSERT_EQ( Vector2f  ().x, 0 );
	ASSERT_EQ( Vector2f  ().y, 0 );
	ASSERT_EQ( Vector2d  ().x, 0 );
	ASSERT_EQ( Vector2d  ().y, 0 );
	ASSERT_EQ( Vector2i8 ().x, 0 );
	ASSERT_EQ( Vector2i8 ().y, 0 );
	ASSERT_EQ( Vector2i16().x, 0 );
	ASSERT_EQ( Vector2i16().y, 0 );
	ASSERT_EQ( Vector2i32().x, 0 );
	ASSERT_EQ( Vector2i32().y, 0 );
	ASSERT_EQ( Vector2i64().x, 0 );
	ASSERT_EQ( Vector2i64().y, 0 );
}



TEST( Vector2, ConstAddVec_CommonTypes )
{
	ASSERT_EQ( Vector2f  ( 1, -1 ) + Vector2f  ( -2, 2 ), Vector2f  ( -1, 1 ) );
	ASSERT_EQ( Vector2d  ( 1, -1 ) + Vector2d  ( -2, 2 ), Vector2d  ( -1, 1 ) );
	ASSERT_EQ( Vector2i8 ( 1, -1 ) + Vector2i8 ( -2, 2 ), Vector2i8 ( -1, 1 ) );
	ASSERT_EQ( Vector2i16( 1, -1 ) + Vector2i16( -2, 2 ), Vector2i16( -1, 1 ) );
	ASSERT_EQ( Vector2i32( 1, -1 ) + Vector2i32( -2, 2 ), Vector2i32( -1, 1 ) );
	ASSERT_EQ( Vector2i64( 1, -1 ) + Vector2i64( -2, 2 ), Vector2i64( -1, 1 ) );
}



TEST( Vector2, ConstSubtractVec_CommonTypes )
{
	ASSERT_EQ( Vector2f  ( 1, -1 ) - Vector2f  ( -2, 2 ), Vector2f  ( 3, -3 ) );
	ASSERT_EQ( Vector2d  ( 1, -1 ) - Vector2d  ( -2, 2 ), Vector2d  ( 3, -3 ) );
	ASSERT_EQ( Vector2i8 ( 1, -1 ) - Vector2i8 ( -2, 2 ), Vector2i8 ( 3, -3 ) );
	ASSERT_EQ( Vector2i16( 1, -1 ) - Vector2i16( -2, 2 ), Vector2i16( 3, -3 ) );
	ASSERT_EQ( Vector2i32( 1, -1 ) - Vector2i32( -2, 2 ), Vector2i32( 3, -3 ) );
	ASSERT_EQ( Vector2i64( 1, -1 ) - Vector2i64( -2, 2 ), Vector2i64( 3, -3 ) );
}



TEST( Vector2, ThisAddVec_CommonTypes )
{
	ASSERT_EQ( Vector2f  ( 1, -1 ) += Vector2f  ( -2, 2 ), Vector2f  ( -1, 1 ) );
	ASSERT_EQ( Vector2d  ( 1, -1 ) += Vector2d  ( -2, 2 ), Vector2d  ( -1, 1 ) );
	ASSERT_EQ( Vector2i8 ( 1, -1 ) += Vector2i8 ( -2, 2 ), Vector2i8 ( -1, 1 ) );
	ASSERT_EQ( Vector2i16( 1, -1 ) += Vector2i16( -2, 2 ), Vector2i16( -1, 1 ) );
	ASSERT_EQ( Vector2i32( 1, -1 ) += Vector2i32( -2, 2 ), Vector2i32( -1, 1 ) );
	ASSERT_EQ( Vector2i64( 1, -1 ) += Vector2i64( -2, 2 ), Vector2i64( -1, 1 ) );
}



TEST( Vector2, ThisSubtractVec_CommonTypes )
{
	ASSERT_EQ( Vector2f  ( 1, -1 ) -= Vector2f  ( -2, 2 ), Vector2f  ( 3, -3 ) );
	ASSERT_EQ( Vector2d  ( 1, -1 ) -= Vector2d  ( -2, 2 ), Vector2d  ( 3, -3 ) );
	ASSERT_EQ( Vector2i8 ( 1, -1 ) -= Vector2i8 ( -2, 2 ), Vector2i8 ( 3, -3 ) );
	ASSERT_EQ( Vector2i16( 1, -1 ) -= Vector2i16( -2, 2 ), Vector2i16( 3, -3 ) );
	ASSERT_EQ( Vector2i32( 1, -1 ) -= Vector2i32( -2, 2 ), Vector2i32( 3, -3 ) );
	ASSERT_EQ( Vector2i64( 1, -1 ) -= Vector2i64( -2, 2 ), Vector2i64( 3, -3 ) );
}



TEST( Vector2, ConstAddScalar_CommonTypes )
{
	ASSERT_EQ( Vector2f  ( 1, -1 ) + 2, Vector2f  ( 3, 1 ) );
	ASSERT_EQ( Vector2d  ( 1, -1 ) + 2, Vector2d  ( 3, 1 ) );
	ASSERT_EQ( Vector2i8 ( 1, -1 ) + 2, Vector2i8 ( 3, 1 ) );
	ASSERT_EQ( Vector2i16( 1, -1 ) + 2, Vector2i16( 3, 1 ) );
	ASSERT_EQ( Vector2i32( 1, -1 ) + 2, Vector2i32( 3, 1 ) );
	ASSERT_EQ( Vector2i64( 1, -1 ) + 2, Vector2i64( 3, 1 ) );
}



TEST( Vector2, ConstSubtractScalar_CommonTypes )
{
	ASSERT_EQ( Vector2f  ( 1, -1 ) - 2, Vector2f  ( -1, -3 ) );
	ASSERT_EQ( Vector2d  ( 1, -1 ) - 2, Vector2d  ( -1, -3 ) );
	ASSERT_EQ( Vector2i8 ( 1, -1 ) - 2, Vector2i8 ( -1, -3 ) );
	ASSERT_EQ( Vector2i16( 1, -1 ) - 2, Vector2i16( -1, -3 ) );
	ASSERT_EQ( Vector2i32( 1, -1 ) - 2, Vector2i32( -1, -3 ) );
	ASSERT_EQ( Vector2i64( 1, -1 ) - 2, Vector2i64( -1, -3 ) );
}



TEST( Vector2, ThisAddScalar_CommonTypes )
{
	ASSERT_EQ( Vector2f  ( 1, -1 ) += 2, Vector2f  ( 3, 1 ) );
	ASSERT_EQ( Vector2d  ( 1, -1 ) += 2, Vector2d  ( 3, 1 ) );
	ASSERT_EQ( Vector2i8 ( 1, -1 ) += 2, Vector2i8 ( 3, 1 ) );
	ASSERT_EQ( Vector2i16( 1, -1 ) += 2, Vector2i16( 3, 1 ) );
	ASSERT_EQ( Vector2i32( 1, -1 ) += 2, Vector2i32( 3, 1 ) );
	ASSERT_EQ( Vector2i64( 1, -1 ) += 2, Vector2i64( 3, 1 ) );
}



TEST( Vector2, ThisSubtractScalar_CommonTypes )
{
	ASSERT_EQ( Vector2f  ( 1, -1 ) -= 2, Vector2f  ( -1, -3 ) );
	ASSERT_EQ( Vector2d  ( 1, -1 ) -= 2, Vector2d  ( -1, -3 ) );
	ASSERT_EQ( Vector2i8 ( 1, -1 ) -= 2, Vector2i8 ( -1, -3 ) );
	ASSERT_EQ( Vector2i16( 1, -1 ) -= 2, Vector2i16( -1, -3 ) );
	ASSERT_EQ( Vector2i32( 1, -1 ) -= 2, Vector2i32( -1, -3 ) );
	ASSERT_EQ( Vector2i64( 1, -1 ) -= 2, Vector2i64( -1, -3 ) );
}

///////////////////////////////////////////////////////////////////////////////
}}
