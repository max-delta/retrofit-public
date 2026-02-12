#pragma once
#include "core_math/MathFwd.h"

#include "rftl/cstdint"
#include "rftl/type_traits"

namespace RF::math {
///////////////////////////////////////////////////////////////////////////////

class Color4u8 final
{
	//
	// Types
public:
	using ElementType = uint8_t;


	//
	// Static colors
public:
	static Color4u8 const kBlack;
	static Color4u8 const kGray25;
	static Color4u8 const kGray50;
	static Color4u8 const kGray75;
	static Color4u8 const kWhite;

	static Color4u8 const kRed;
	static Color4u8 const kGreen;
	static Color4u8 const kBlue;

	static Color4u8 const kCyan;
	static Color4u8 const kMagenta;
	static Color4u8 const kYellow;

	static Color4u8 const kTeal;
	static Color4u8 const kPurple;
	static Color4u8 const kOrange;


	//
	// Public methods
public:
	Color4u8();
	Color4u8( ElementType r, ElementType g, ElementType b );
	Color4u8( ElementType r, ElementType g, ElementType b, ElementType a );
	explicit Color4u8( ElementType const ( &rgba )[4] );
	explicit Color4u8( Color3u8 rgb );
	Color4u8( Color3u8 rgb, ElementType a );
	explicit Color4u8( Color3f rgb );

	static Color4u8 RandomFromHash( uint64_t hashVal, ElementType a );

	bool operator==( Color4u8 const& rhs ) const;

	Color4u8 operator+( Color4u8 const& rhs ) const;
	Color4u8 operator-( Color4u8 const& rhs ) const;
	Color4u8& operator+=( Color4u8 const& rhs );
	Color4u8& operator-=( Color4u8 const& rhs );

	Color4u8 operator+( ElementType const& rhs ) const;
	Color4u8 operator-( ElementType const& rhs ) const;
	Color4u8 operator*( ElementType const& rhs ) const;
	Color4u8& operator+=( ElementType const& rhs );
	Color4u8& operator-=( ElementType const& rhs );
	Color4u8& operator*=( ElementType const& rhs );


	//
	// Public data
public:
	ElementType r = 0;
	ElementType g = 0;
	ElementType b = 0;
	ElementType a = 0;
};

static_assert( sizeof( Color4u8 ) == 4, "Unexpected size" );
static_assert( alignof( Color4u8 ) == 1, "Unexpected size" );

///////////////////////////////////////////////////////////////////////////////
}
