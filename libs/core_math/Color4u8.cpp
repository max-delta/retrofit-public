#include "stdafx.h"
#include "Color4u8.h"

#include "core_math/math_compare.h"
#include "core_math/math_casts.h"
#include "core_math/Rand.h"


namespace RF { namespace math {
///////////////////////////////////////////////////////////////////////////////

Color4u8 const Color4u8::kBlack{ 0, 0, 0 };
Color4u8 const Color4u8::kGray25{ 64, 64, 64 };
Color4u8 const Color4u8::kGray50{ 127, 127, 127 };
Color4u8 const Color4u8::kGray75{ 192, 192, 192 };
Color4u8 const Color4u8::kWhite{ 255, 255, 255 };

Color4u8 const Color4u8::kRed{ 255, 0, 0 };
Color4u8 const Color4u8::kGreen{ 0, 255, 0 };
Color4u8 const Color4u8::kBlue{ 0, 0, 255 };

Color4u8 const Color4u8::kCyan{ 0, 255, 255 };
Color4u8 const Color4u8::kMagenta{ 255, 0, 255 };
Color4u8 const Color4u8::kYellow{ 255, 255, 0 };

///////////////////////////////////////////////////////////////////////////////

Color4u8::Color4u8()
	: r( 0 )
	, g( 0 )
	, b( 0 )
	, a( 0 )
{
	//
}



Color4u8::Color4u8( ElementType r, ElementType g, ElementType b )
	: r( r )
	, g( g )
	, b( b )
	, a( 255 )
{
	//
}



Color4u8::Color4u8( ElementType r, ElementType g, ElementType b, ElementType a )
	: r( r )
	, g( g )
	, b( b )
	, a( a )
{
	//
}



Color4u8::Color4u8( ElementType const ( &rgba )[4] )
{
	// Raw write as uint32_t
	// NOTE: Native endianness used on read+write, so should cancel out
	static_assert( sizeof( Color4u8 ) == sizeof( uint32_t ), "Unexpected size" );
	static_assert( sizeof( ElementType const[4] ) == sizeof( uint32_t ), "Unexpected size" );
	*reinterpret_cast<uint32_t*>( this ) = reinterpret_cast<uint32_t const&>( rgba );
}



Color4u8 Color4u8::RandomFromHash( uint64_t hashVal, ElementType a )
{
	uint32_t seed = GetSeedFromHash( hashVal );
	return Color4u8(
		StableRandLCG( seed ) & 0xff,
		StableRandLCG( seed ) & 0xff,
		StableRandLCG( seed ) & 0xff,
		a );
}



bool Color4u8::operator==( Color4u8 const& rhs ) const
{
	return
		Equals( r, rhs.r ) &&
		Equals( g, rhs.g ) &&
		Equals( b, rhs.b ) &&
		Equals( a, rhs.a );
}



Color4u8 Color4u8::operator+( Color4u8 const& rhs ) const
{
	return Color4u8(
		integer_cast<ElementType>( r + rhs.r ),
		integer_cast<ElementType>( g + rhs.g ),
		integer_cast<ElementType>( b + rhs.b ),
		integer_cast<ElementType>( a + rhs.a ) );
}



Color4u8 Color4u8::operator-( Color4u8 const& rhs ) const
{
	return Color4u8(
		integer_cast<ElementType>( r - rhs.r ),
		integer_cast<ElementType>( g - rhs.g ),
		integer_cast<ElementType>( b - rhs.b ),
		integer_cast<ElementType>( a - rhs.a ) );
}



Color4u8& Color4u8::operator+=( Color4u8 const& rhs )
{
	r += rhs.r;
	g += rhs.g;
	b += rhs.b;
	a += rhs.a;
	return *this;
}



Color4u8& Color4u8::operator-=( Color4u8 const& rhs )
{
	r -= rhs.r;
	g -= rhs.g;
	b -= rhs.b;
	a -= rhs.a;
	return *this;
}



Color4u8 Color4u8::operator+( ElementType const& rhs ) const
{
	return Color4u8(
		integer_cast<ElementType>( r + rhs ),
		integer_cast<ElementType>( g + rhs ),
		integer_cast<ElementType>( b + rhs ),
		integer_cast<ElementType>( a + rhs ) );
}



Color4u8 Color4u8::operator-( ElementType const& rhs ) const
{
	return Color4u8(
		integer_cast<ElementType>( r - rhs ),
		integer_cast<ElementType>( g - rhs ),
		integer_cast<ElementType>( b - rhs ),
		integer_cast<ElementType>( a - rhs ) );
}



Color4u8 Color4u8::operator*( ElementType const& rhs ) const
{
	return Color4u8(
		integer_cast<ElementType>( r * rhs ),
		integer_cast<ElementType>( g * rhs ),
		integer_cast<ElementType>( b * rhs ),
		integer_cast<ElementType>( a * rhs ) );
}



Color4u8& Color4u8::operator+=( ElementType const& rhs )
{
	r = integer_cast<ElementType>( r + rhs );
	g = integer_cast<ElementType>( g + rhs );
	b = integer_cast<ElementType>( b + rhs );
	a = integer_cast<ElementType>( a + rhs );
	return *this;
}



Color4u8& Color4u8::operator-=( ElementType const& rhs )
{
	r = integer_cast<ElementType>( r - rhs );
	g = integer_cast<ElementType>( g - rhs );
	b = integer_cast<ElementType>( b - rhs );
	a = integer_cast<ElementType>( a - rhs );
	return *this;
}



Color4u8& Color4u8::operator*=( ElementType const& rhs )
{
	r = integer_cast<ElementType>( r * rhs );
	g = integer_cast<ElementType>( g * rhs );
	b = integer_cast<ElementType>( b * rhs );
	a = integer_cast<ElementType>( a * rhs );
	return *this;
}

///////////////////////////////////////////////////////////////////////////////
}}
