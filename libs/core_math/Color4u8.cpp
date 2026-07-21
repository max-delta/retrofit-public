#include "stdafx.h"
#include "Color4u8.h"

#include "core_math/Color3f.h"
#include "core_math/Color3u8.h"
#include "core_math/Color4a5.h"
#include "core_math/ColorSource.h"
#include "core_math/math_compare.h"
#include "core_math/math_casts.h"


namespace RF::math {
///////////////////////////////////////////////////////////////////////////////

Color4u8 const Color4u8::kBlack{ details::color_source::kBlack };
Color4u8 const Color4u8::kGray25{ details::color_source::kGray25 };
Color4u8 const Color4u8::kGray50{ details::color_source::kGray50 };
Color4u8 const Color4u8::kGray75{ details::color_source::kGray75 };
Color4u8 const Color4u8::kWhite{ details::color_source::kWhite };

Color4u8 const Color4u8::kRed{ details::color_source::kRed };
Color4u8 const Color4u8::kGreen{ details::color_source::kGreen };
Color4u8 const Color4u8::kBlue{ details::color_source::kBlue };

Color4u8 const Color4u8::kCyan{ details::color_source::kCyan };
Color4u8 const Color4u8::kMagenta{ details::color_source::kMagenta };
Color4u8 const Color4u8::kYellow{ details::color_source::kYellow };

Color4u8 const Color4u8::kTeal{ details::color_source::kTeal };
Color4u8 const Color4u8::kPurple{ details::color_source::kPurple };
Color4u8 const Color4u8::kOrange{ details::color_source::kOrange };


///////////////////////////////////////////////////////////////////////////////

Color4u8::Color4u8() = default;



Color4u8::Color4u8( ElementType r, ElementType g, ElementType b )
	: Color4u8( r, g, b, 255 )
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
	*reinterpret_cast<uint32_t*>( this ) = *reinterpret_cast<uint32_t const*>( &( rgba[0] ) );
}



Color4u8::Color4u8( Color3f rgb )
	: Color4u8( Color3u8( rgb ) )
{
	//
}



Color4u8::Color4u8( Color3u8 rgb )
	: Color4u8( rgb, 255 )
{
	//
}



Color4u8::Color4u8( Color3u8 rgb, ElementType a )
	: Color4u8( rgb.r, rgb.g, rgb.b, a )
{
	//
}



Color4u8::Color4u8( Color4a5 rgba )
	: Color4u8( rgba.r(), rgba.g(), rgba.b(), rgba.a() )
{
	//
}



Color4u8 Color4u8::RandomFromHash( uint64_t hashVal, ElementType a )
{
	return Color4u8( Color3u8::RandomFromHash( hashVal ), a );
}



bool Color4u8::operator==( Color4u8 const& rhs ) const
{
	return Equals( r, rhs.r ) &&
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
	*this = *this + rhs;
	return *this;
}



Color4u8& Color4u8::operator-=( Color4u8 const& rhs )
{
	*this = *this - rhs;
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
	*this = *this + rhs;
	return *this;
}



Color4u8& Color4u8::operator-=( ElementType const& rhs )
{
	*this = *this - rhs;
	return *this;
}



Color4u8& Color4u8::operator*=( ElementType const& rhs )
{
	*this = *this * rhs;
	return *this;
}

///////////////////////////////////////////////////////////////////////////////
}
