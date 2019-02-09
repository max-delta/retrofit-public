#include "stdafx.h"
#include "Color3f.h"

#include "core_math/math_compare.h"
#include "core_math/math_clamps.h"


namespace RF { namespace math {
///////////////////////////////////////////////////////////////////////////////

inline Color3f::Color3f()
	: r( 0.f )
	, g( 0.f )
	, b( 0.f )
{
	//
}



inline Color3f::Color3f( ElementType r, ElementType g, ElementType b )
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



inline bool Color3f::operator==( Color3f const& rhs ) const
{
	return Equals( r, rhs.r ) && Equals( g, rhs.g ) && Equals( b, rhs.b );
}



inline Color3f Color3f::operator+( Color3f const& rhs ) const
{
	return Color3f( r + rhs.r, g + rhs.g, b + rhs.b );
}



inline Color3f Color3f::operator-( Color3f const& rhs ) const
{
	return Color3f( r - rhs.r, g - rhs.g, b - rhs.b );
}



inline Color3f& Color3f::operator+=( Color3f const& rhs )
{
	r += rhs.r;
	g += rhs.g;
	b += rhs.b;
	return *this;
}



inline Color3f& Color3f::operator-=( Color3f const& rhs )
{
	r -= rhs.r;
	g -= rhs.g;
	b -= rhs.b;
	return *this;
}



inline Color3f Color3f::operator+( ElementType const& rhs ) const
{
	return Color3f( r + rhs, g + rhs, b + rhs );
}



inline Color3f Color3f::operator-( ElementType const& rhs ) const
{
	return Color3f( r - rhs, g - rhs, b - rhs );
}



inline Color3f Color3f::operator*( ElementType const& rhs ) const
{
	return Color3f( r * rhs, g * rhs, b * rhs );
}



inline Color3f& Color3f::operator+=( ElementType const& rhs )
{
	r += rhs;
	g += rhs;
	b += rhs;
	return *this;
}



inline Color3f& Color3f::operator-=( ElementType const& rhs )
{
	r -= rhs;
	g -= rhs;
	b -= rhs;
	return *this;
}



inline Color3f& Color3f::operator*=( ElementType const& rhs )
{
	r *= rhs;
	g *= rhs;
	b *= rhs;
	return *this;
}

///////////////////////////////////////////////////////////////////////////////
}}
