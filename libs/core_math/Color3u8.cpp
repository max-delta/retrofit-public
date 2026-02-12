#include "stdafx.h"
#include "Color3u8.h"

#include "core_math/Color3f.h"
#include "core_math/Color4u8.h"
#include "core_math/math_compare.h"
#include "core_math/math_casts.h"
#include "core_math/Rand.h"


namespace RF::math {
///////////////////////////////////////////////////////////////////////////////

Color3u8 const Color3u8::kBlack{ 0, 0, 0 };
Color3u8 const Color3u8::kGray25{ 64, 64, 64 };
Color3u8 const Color3u8::kGray50{ 127, 127, 127 };
Color3u8 const Color3u8::kGray75{ 192, 192, 192 };
Color3u8 const Color3u8::kWhite{ 255, 255, 255 };

Color3u8 const Color3u8::kRed{ 255, 0, 0 };
Color3u8 const Color3u8::kGreen{ 0, 255, 0 };
Color3u8 const Color3u8::kBlue{ 0, 0, 255 };

Color3u8 const Color3u8::kCyan{ 0, 255, 255 };
Color3u8 const Color3u8::kMagenta{ 255, 0, 255 };
Color3u8 const Color3u8::kYellow{ 255, 255, 0 };

Color3u8 const Color3u8::kTeal{ 0, 255, 127 };
Color3u8 const Color3u8::kPurple{ 127, 0, 255 };
Color3u8 const Color3u8::kOrange{ 255, 127, 0 };

///////////////////////////////////////////////////////////////////////////////

Color3u8::Color3u8() = default;



Color3u8::Color3u8( ElementType r, ElementType g, ElementType b )
	: r( r )
	, g( g )
	, b( b )
{
	//
}



Color3u8::Color3u8( ElementType const ( &rgb )[3] )
	: Color3u8( rgb[0], rgb[1], rgb[2] )
{
	//
}



Color3u8::Color3u8( Color4u8 rgba )
	: Color3u8( rgba.r, rgba.g, rgba.b )
{
	//
}



Color3u8::Color3u8( Color3f rgb )
	: Color3u8(
		  integer_cast<ElementType>( rgb.r * float_cast<float>( rftl::numeric_limits<ElementType>::max() ) ),
		  integer_cast<ElementType>( rgb.g * float_cast<float>( rftl::numeric_limits<ElementType>::max() ) ),
		  integer_cast<ElementType>( rgb.b * float_cast<float>( rftl::numeric_limits<ElementType>::max() ) ) )
{
	//
}



Color3u8 Color3u8::RandomFromHash( uint64_t hashVal )
{
	uint32_t const seed = GetSeedFromHash( hashVal );
	uint32_t const r = StableRandLCG( seed );
	uint32_t const g = StableRandLCG( r );
	uint32_t const b = StableRandLCG( g );
	return Color3u8(
		r & 0xff,
		g & 0xff,
		b & 0xff );
}



bool Color3u8::operator==( Color3u8 const& rhs ) const
{
	return Equals( r, rhs.r ) &&
		Equals( g, rhs.g ) &&
		Equals( b, rhs.b );
}



Color3u8 Color3u8::operator+( Color3u8 const& rhs ) const
{
	return Color3u8(
		integer_cast<ElementType>( r + rhs.r ),
		integer_cast<ElementType>( g + rhs.g ),
		integer_cast<ElementType>( b + rhs.b ) );
}



Color3u8 Color3u8::operator-( Color3u8 const& rhs ) const
{
	return Color3u8(
		integer_cast<ElementType>( r - rhs.r ),
		integer_cast<ElementType>( g - rhs.g ),
		integer_cast<ElementType>( b - rhs.b ) );
}



Color3u8& Color3u8::operator+=( Color3u8 const& rhs )
{
	r += rhs.r;
	g += rhs.g;
	b += rhs.b;
	return *this;
}



Color3u8& Color3u8::operator-=( Color3u8 const& rhs )
{
	r -= rhs.r;
	g -= rhs.g;
	b -= rhs.b;
	return *this;
}



Color3u8 Color3u8::operator+( ElementType const& rhs ) const
{
	return Color3u8(
		integer_cast<ElementType>( r + rhs ),
		integer_cast<ElementType>( g + rhs ),
		integer_cast<ElementType>( b + rhs ) );
}



Color3u8 Color3u8::operator-( ElementType const& rhs ) const
{
	return Color3u8(
		integer_cast<ElementType>( r - rhs ),
		integer_cast<ElementType>( g - rhs ),
		integer_cast<ElementType>( b - rhs ) );
}



Color3u8 Color3u8::operator*( ElementType const& rhs ) const
{
	return Color3u8(
		integer_cast<ElementType>( r * rhs ),
		integer_cast<ElementType>( g * rhs ),
		integer_cast<ElementType>( b * rhs ) );
}



Color3u8& Color3u8::operator+=( ElementType const& rhs )
{
	r = integer_cast<ElementType>( r + rhs );
	g = integer_cast<ElementType>( g + rhs );
	b = integer_cast<ElementType>( b + rhs );
	return *this;
}



Color3u8& Color3u8::operator-=( ElementType const& rhs )
{
	r = integer_cast<ElementType>( r - rhs );
	g = integer_cast<ElementType>( g - rhs );
	b = integer_cast<ElementType>( b - rhs );
	return *this;
}



Color3u8& Color3u8::operator*=( ElementType const& rhs )
{
	r = integer_cast<ElementType>( r * rhs );
	g = integer_cast<ElementType>( g * rhs );
	b = integer_cast<ElementType>( b * rhs );
	return *this;
}

///////////////////////////////////////////////////////////////////////////////
}
