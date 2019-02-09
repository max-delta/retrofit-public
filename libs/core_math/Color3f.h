#pragma once
#include "rftl/cstdint"
#include "rftl/type_traits"

namespace RF { namespace math {
///////////////////////////////////////////////////////////////////////////////

class Color3f final
{
	//
	// Types
public:
	using ElementType = float;


	//
	// Static colors
public:
	static Color3f const kBlack;
	static Color3f const kGray25;
	static Color3f const kGray50;
	static Color3f const kGray75;
	static Color3f const kWhite;

	static Color3f const kRed;
	static Color3f const kGreen;
	static Color3f const kBlue;
	static Color3f const kCyan;

	static Color3f const kMagenta;
	static Color3f const kYellow;


	//
	// Public methods
public:
	Color3f();
	Color3f( ElementType x, ElementType y, ElementType z );

	static Color3f RandomFromHash( uint64_t hashVal );

	Color3f& Clamp();
	Color3f& Clamp( ElementType min, ElementType max );
	Color3f& Clamp( Color3f const& min, Color3f const& max );

	bool operator==( Color3f const& rhs ) const;

	Color3f operator+( Color3f const& rhs ) const;
	Color3f operator-( Color3f const& rhs ) const;
	Color3f& operator+=( Color3f const& rhs );
	Color3f& operator-=( Color3f const& rhs );

	Color3f operator+( ElementType const& rhs ) const;
	Color3f operator-( ElementType const& rhs ) const;
	Color3f operator*( ElementType const& rhs ) const;
	Color3f& operator+=( ElementType const& rhs );
	Color3f& operator-=( ElementType const& rhs );
	Color3f& operator*=( ElementType const& rhs );


	//
	// Public data
public:
	ElementType r;
	ElementType g;
	ElementType b;
};

///////////////////////////////////////////////////////////////////////////////
}}
