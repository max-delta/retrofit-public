#include "stdafx.h"
#include "Color3f.h"

#include "core_math/math_compare.h"
#include "core_math/math_clamps.h"


namespace RF { namespace math {
///////////////////////////////////////////////////////////////////////////////

Color3f const Color3f::kBlack{ 0.f, 0.f, 0.f };
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



void Color3f::Clamp()
{
	r = math::Clamp( 0.f, r, 1.f );
	g = math::Clamp( 0.f, g, 1.f );
	b = math::Clamp( 0.f, b, 1.f );
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
