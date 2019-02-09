#include "stdafx.h"
#include "Color3f.h"

#include "core_math/math_compare.h"
#include "core_math/math_clamps.h"
#include "core_math/Rand.h"


namespace RF { namespace math {
///////////////////////////////////////////////////////////////////////////////

Color3f const Color3f::kBlack{ 0.f, 0.f, 0.f };
Color3f const Color3f::kGray25{ 0.25f, 0.25f, 0.25f };
Color3f const Color3f::kGray50{ 0.50f, 0.50f, 0.50f };
Color3f const Color3f::kGray75{ 0.75f, 0.75f, 0.75f };
Color3f const Color3f::kWhite{ 1.f, 1.f, 1.f };

Color3f const Color3f::kRed{ 1.f, 0.f, 0.f };
Color3f const Color3f::kGreen{ 0.f, 1.f, 0.f };
Color3f const Color3f::kBlue{ 0.f, 0.f, 1.f };
Color3f const Color3f::kCyan{ 0.f, 1.f, 1.f };

Color3f const Color3f::kMagenta{ 1.f, 0.f, 1.f };
Color3f const Color3f::kYellow{ 1.f, 1.f, 0.f };

///////////////////////////////////////////////////////////////////////////////

Color3f::Color3f()
	: r( 0.f )
	, g( 0.f )
	, b( 0.f )
{
	//
}



Color3f::Color3f( ElementType r, ElementType g, ElementType b )
	: r( r )
	, g( g )
	, b( b )
{
	//
}



Color3f Color3f::RandomFromHash( uint64_t hashVal )
{
	uint32_t seed = GetSeedFromHash( hashVal );
	return Color3f(
		StableRandLCGPercent( seed ) / 100.f,
		StableRandLCGPercent( seed ) / 100.f,
		StableRandLCGPercent( seed ) / 100.f );
}



Color3f& Color3f::Clamp()
{
	Clamp( 0.f, 1.f );
	return *this;
}



Color3f& Color3f::Clamp( ElementType min, ElementType max )
{
	r = math::Clamp( min, r, max );
	g = math::Clamp( min, g, max );
	b = math::Clamp( min, b, max );
	return *this;
}



Color3f& Color3f::Clamp( Color3f const& min, Color3f const& max )
{
	r = math::Clamp( min.r, r, max.r );
	g = math::Clamp( min.g, g, max.g );
	b = math::Clamp( min.b, b, max.b );
	return *this;
}



bool Color3f::operator==( Color3f const& rhs ) const
{
	return Equals( r, rhs.r ) && Equals( g, rhs.g ) && Equals( b, rhs.b );
}



Color3f Color3f::operator+( Color3f const& rhs ) const
{
	return Color3f( r + rhs.r, g + rhs.g, b + rhs.b );
}



Color3f Color3f::operator-( Color3f const& rhs ) const
{
	return Color3f( r - rhs.r, g - rhs.g, b - rhs.b );
}



Color3f& Color3f::operator+=( Color3f const& rhs )
{
	r += rhs.r;
	g += rhs.g;
	b += rhs.b;
	return *this;
}



Color3f& Color3f::operator-=( Color3f const& rhs )
{
	r -= rhs.r;
	g -= rhs.g;
	b -= rhs.b;
	return *this;
}



Color3f Color3f::operator+( ElementType const& rhs ) const
{
	return Color3f( r + rhs, g + rhs, b + rhs );
}



Color3f Color3f::operator-( ElementType const& rhs ) const
{
	return Color3f( r - rhs, g - rhs, b - rhs );
}



Color3f Color3f::operator*( ElementType const& rhs ) const
{
	return Color3f( r * rhs, g * rhs, b * rhs );
}



Color3f& Color3f::operator+=( ElementType const& rhs )
{
	r += rhs;
	g += rhs;
	b += rhs;
	return *this;
}



Color3f& Color3f::operator-=( ElementType const& rhs )
{
	r -= rhs;
	g -= rhs;
	b -= rhs;
	return *this;
}



Color3f& Color3f::operator*=( ElementType const& rhs )
{
	r *= rhs;
	g *= rhs;
	b *= rhs;
	return *this;
}

///////////////////////////////////////////////////////////////////////////////
}}
