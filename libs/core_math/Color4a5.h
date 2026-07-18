#pragma once
#include "core_math/MathFwd.h"


namespace RF::math {
///////////////////////////////////////////////////////////////////////////////

class Color4a5 final
{
	//
	// Types
public:
	using ElementType = uint8_t;


	//
	// Static colors
public:
	static Color4a5 const kBlack;
	static Color4a5 const kGray25;
	static Color4a5 const kGray50;
	static Color4a5 const kGray75;
	static Color4a5 const kWhite;

	static Color4a5 const kRed;
	static Color4a5 const kGreen;
	static Color4a5 const kBlue;

	static Color4a5 const kCyan;
	static Color4a5 const kMagenta;
	static Color4a5 const kYellow;

	static Color4a5 const kTeal;
	static Color4a5 const kPurple;
	static Color4a5 const kOrange;


	//
	// Public methods
public:
	Color4a5();
	Color4a5( ElementType r, ElementType g, ElementType b );
	Color4a5( ElementType r, ElementType g, ElementType b, ElementType a );
	explicit Color4a5( uint8_t const ( &data )[2] );
	explicit Color4a5( Color3f rgb );
	explicit Color4a5( Color3u8 rgb );
	Color4a5( Color3u8 rgb, ElementType a );
	explicit Color4a5( Color4u8 rgba );

	static Color4a5 RandomFromHash( uint64_t hashVal );

	bool operator==( Color4a5 const& rhs ) const;

	Color4a5 operator+( Color4a5 const& rhs ) const;
	Color4a5 operator-( Color4a5 const& rhs ) const;
	Color4a5& operator+=( Color4a5 const& rhs );
	Color4a5& operator-=( Color4a5 const& rhs );

	Color4a5 operator+( ElementType const& rhs ) const;
	Color4a5 operator-( ElementType const& rhs ) const;
	Color4a5 operator*( ElementType const& rhs ) const;
	Color4a5& operator+=( ElementType const& rhs );
	Color4a5& operator-=( ElementType const& rhs );
	Color4a5& operator*=( ElementType const& rhs );

	uint16_t Data() const;
	ElementType r() const;
	ElementType g() const;
	ElementType b() const;
	ElementType a() const;


	//
	// Private data
private:
	uint8_t mHigh = 0;
	uint8_t mLow = 0;
};

static_assert( sizeof( Color4a5 ) == 2, "Unexpected size" );
static_assert( alignof( Color4a5 ) == 1, "Unexpected size" );

///////////////////////////////////////////////////////////////////////////////
}
