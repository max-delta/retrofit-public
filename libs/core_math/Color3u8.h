#pragma once
#include "core_math/MathFwd.h"

#include "rftl/cstdint"
#include "rftl/type_traits"

namespace RF::math {
///////////////////////////////////////////////////////////////////////////////

class Color3u8 final
{
	//
	// Types
public:
	using ElementType = uint8_t;


	//
	// Static colors
public:
	static Color3u8 const kBlack;
	static Color3u8 const kGray25;
	static Color3u8 const kGray50;
	static Color3u8 const kGray75;
	static Color3u8 const kWhite;

	static Color3u8 const kRed;
	static Color3u8 const kGreen;
	static Color3u8 const kBlue;

	static Color3u8 const kCyan;
	static Color3u8 const kMagenta;
	static Color3u8 const kYellow;

	static Color3u8 const kTeal;
	static Color3u8 const kPurple;
	static Color3u8 const kOrange;


	//
	// Public methods
public:
	Color3u8();
	Color3u8( ElementType r, ElementType g, ElementType b );
	explicit Color3u8( ElementType const ( &rgb )[3] );
	explicit Color3u8( Color4u8 rgba );
	explicit Color3u8( Color3f rgb );

	static Color3u8 RandomFromHash( uint64_t hashVal );

	bool operator==( Color3u8 const& rhs ) const;

	Color3u8 operator+( Color3u8 const& rhs ) const;
	Color3u8 operator-( Color3u8 const& rhs ) const;
	Color3u8& operator+=( Color3u8 const& rhs );
	Color3u8& operator-=( Color3u8 const& rhs );

	Color3u8 operator+( ElementType const& rhs ) const;
	Color3u8 operator-( ElementType const& rhs ) const;
	Color3u8 operator*( ElementType const& rhs ) const;
	Color3u8& operator+=( ElementType const& rhs );
	Color3u8& operator-=( ElementType const& rhs );
	Color3u8& operator*=( ElementType const& rhs );


	//
	// Public data
public:
	ElementType r = 0;
	ElementType g = 0;
	ElementType b = 0;
};

static_assert( sizeof( Color3u8 ) == 3, "Unexpected size" );
static_assert( alignof( Color3u8 ) == 1, "Unexpected size" );

///////////////////////////////////////////////////////////////////////////////
}
