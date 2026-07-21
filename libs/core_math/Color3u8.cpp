#include "stdafx.h"
#include "Color3u8.h"

#include "core_math/Color3f.h"
#include "core_math/Color4a5.h"
#include "core_math/Color4u8.h"
#include "core_math/ColorSource.h"
#include "core_math/math_compare.h"
#include "core_math/math_casts.h"
#include "core_math/Rand.h"


namespace RF::math {
///////////////////////////////////////////////////////////////////////////////

Color3u8 const Color3u8::kBlack{ details::color_source::kBlack };
Color3u8 const Color3u8::kGray25{ details::color_source::kGray25 };
Color3u8 const Color3u8::kGray50{ details::color_source::kGray50 };
Color3u8 const Color3u8::kGray75{ details::color_source::kGray75 };
Color3u8 const Color3u8::kWhite{ details::color_source::kWhite };

Color3u8 const Color3u8::kRed{ details::color_source::kRed };
Color3u8 const Color3u8::kGreen{ details::color_source::kGreen };
Color3u8 const Color3u8::kBlue{ details::color_source::kBlue };

Color3u8 const Color3u8::kCyan{ details::color_source::kCyan };
Color3u8 const Color3u8::kMagenta{ details::color_source::kMagenta };
Color3u8 const Color3u8::kYellow{ details::color_source::kYellow };

Color3u8 const Color3u8::kTeal{ details::color_source::kTeal };
Color3u8 const Color3u8::kPurple{ details::color_source::kPurple };
Color3u8 const Color3u8::kOrange{ details::color_source::kOrange };

///////////////////////////////////////////////////////////////////////////////

Color3u8::Color3u8() = default;



Color3u8::Color3u8( ElementType const ( &rgb )[3] )
	: Color3u8( rgb[0], rgb[1], rgb[2] )
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



Color3u8::Color3u8( Color4a5 rgba )
	: Color3u8( rgba.r(), rgba.g(), rgba.b() )
{
	//
}



Color3u8::Color3u8( Color4u8 rgba )
	: Color3u8( rgba.r, rgba.g, rgba.b )
{
	//
}



Color3u8 Color3u8::RandomFromHash( uint64_t hashVal )
{
	uint32_t seed = GetSeedFromHash( hashVal );
	return Color3u8(
		StableRandLCGRaw( seed ) & 0xff,
		StableRandLCGRaw( seed ) & 0xff,
		StableRandLCGRaw( seed ) & 0xff );
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
	*this = *this + rhs;
	return *this;
}



Color3u8& Color3u8::operator-=( Color3u8 const& rhs )
{
	*this = *this - rhs;
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
	*this = *this + rhs;
	return *this;
}



Color3u8& Color3u8::operator-=( ElementType const& rhs )
{
	*this = *this - rhs;
	return *this;
}



Color3u8& Color3u8::operator*=( ElementType const& rhs )
{
	*this = *this * rhs;
	return *this;
}

///////////////////////////////////////////////////////////////////////////////
}
