#pragma once
#include "rftl/cstdint"
#include "rftl/type_traits"

namespace RF { namespace math {
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


	//
	// Public methods
public:
	Color4u8();
	Color4u8( ElementType r, ElementType g, ElementType b );
	Color4u8( ElementType r, ElementType g, ElementType b, ElementType a );
	explicit Color4u8( ElementType const ( &rgba )[4] );

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
	ElementType r;
	ElementType g;
	ElementType b;
	ElementType a;
};

static_assert( sizeof( Color4u8 ) == 4, "Unexpected size" );
static_assert( alignof( Color4u8 ) == 1, "Unexpected size" );

///////////////////////////////////////////////////////////////////////////////
}}
