#include "stdafx.h"
#include "Color4a5.h"

#include "core_math/Color3f.h"
#include "core_math/Color3u8.h"
#include "core_math/Color4u8.h"
#include "core_math/ColorSource.h"
#include "core_math/math_compare.h"
#include "core_math/math_casts.h"
#include "core_math/Rand.h"

#include "core/meta/IntegerPromotion.h"
#include "core/meta/SafeCasts.h"


namespace RF::math {
///////////////////////////////////////////////////////////////////////////////
namespace details {

static constexpr uint16_t kMaxValIn8 = 255;
static constexpr uint16_t kMaxValIn5 = 31;

}
///////////////////////////////////////////////////////////////////////////////

Color4a5 const Color4a5::kBlack{ details::color_source::kBlack };
Color4a5 const Color4a5::kGray25{ details::color_source::kGray25 };
Color4a5 const Color4a5::kGray50{ details::color_source::kGray50 };
Color4a5 const Color4a5::kGray75{ details::color_source::kGray75 };
Color4a5 const Color4a5::kWhite{ details::color_source::kWhite };

Color4a5 const Color4a5::kRed{ details::color_source::kRed };
Color4a5 const Color4a5::kGreen{ details::color_source::kGreen };
Color4a5 const Color4a5::kBlue{ details::color_source::kBlue };

Color4a5 const Color4a5::kCyan{ details::color_source::kCyan };
Color4a5 const Color4a5::kMagenta{ details::color_source::kMagenta };
Color4a5 const Color4a5::kYellow{ details::color_source::kYellow };

Color4a5 const Color4a5::kTeal{ details::color_source::kTeal };
Color4a5 const Color4a5::kPurple{ details::color_source::kPurple };
Color4a5 const Color4a5::kOrange{ details::color_source::kOrange };


///////////////////////////////////////////////////////////////////////////////

Color4a5::Color4a5() = default;



Color4a5::Color4a5( ElementType r, ElementType g, ElementType b )
	: Color4a5( r, g, b, 1 )
{
	//
}



Color4a5::Color4a5( ElementType r, ElementType g, ElementType b, ElementType a )
{
	uint8_t const rC = integer_cast<uint8_t>( ( r * details::kMaxValIn5 ) / details::kMaxValIn8 );
	uint8_t const gC = integer_cast<uint8_t>( ( g * details::kMaxValIn5 ) / details::kMaxValIn8 );
	uint8_t const bC = integer_cast<uint8_t>( ( b * details::kMaxValIn5 ) / details::kMaxValIn8 );
	bool const aC = a > 0;
	// rrrrrgggggbbbbba
	// ^^^^^12345678901 (<< 11)
	//      ^^^^^123456 (<< 6)
	//           ^^^^^1 (<< 1)
	uint16_t const data =
		angry_cast<uint16_t>( ( rC & 0b0001'1111u ) << 11u ) |
		angry_cast<uint16_t>( ( gC & 0b0001'1111u ) << 6u ) |
		angry_cast<uint16_t>( ( bC & 0b0001'1111u ) << 1u ) |
		( aC ? 0b1u : 0b0u );
	mHigh = ( data >> 8u ) & 0b1111'1111u;
	mLow = data & 0b1111'1111u;
}



Color4a5::Color4a5( uint8_t const ( &data )[2] )
	: mHigh( data[0] )
	, mLow( data[1] )
{
	//
}



Color4a5::Color4a5( Color3f rgb )
	: Color4a5(
		  integer_cast<ElementType>( rgb.r * float_cast<float>( rftl::numeric_limits<ElementType>::max() ) ),
		  integer_cast<ElementType>( rgb.g * float_cast<float>( rftl::numeric_limits<ElementType>::max() ) ),
		  integer_cast<ElementType>( rgb.b * float_cast<float>( rftl::numeric_limits<ElementType>::max() ) ) )
{
	//
}



Color4a5::Color4a5( Color3u8 rgb )
	: Color4a5( rgb, details::kMaxValIn8 )
{
	//
}



Color4a5::Color4a5( Color3u8 rgb, ElementType a )
	: Color4a5( rgb.r, rgb.g, rgb.b, a )
{
	//
}



Color4a5::Color4a5( Color4u8 rgba )
	: Color4a5( rgba.r, rgba.g, rgba.b, rgba.a )
{
	//
}



Color4a5 Color4a5::RandomFromHash( uint64_t hashVal )
{
	uint32_t seed = GetSeedFromHash( hashVal );
	return Color4a5(
		StableRandLCGRaw( seed ) & 0xff,
		StableRandLCGRaw( seed ) & 0xff,
		StableRandLCGRaw( seed ) & 0xff );
}



bool Color4a5::operator==( Color4a5 const& rhs ) const
{
	return Equals( mHigh, rhs.mHigh ) &&
		Equals( mLow, rhs.mLow );
}



Color4a5 Color4a5::operator+( Color4a5 const& rhs ) const
{
	return Color4a5(
		integer_cast<ElementType>( r() + rhs.r() ),
		integer_cast<ElementType>( g() + rhs.g() ),
		integer_cast<ElementType>( b() + rhs.b() ),
		integer_cast<ElementType>( a() + rhs.a() ) );
}



Color4a5 Color4a5::operator-( Color4a5 const& rhs ) const
{
	return Color4a5(
		integer_cast<ElementType>( r() - rhs.r() ),
		integer_cast<ElementType>( g() - rhs.g() ),
		integer_cast<ElementType>( b() - rhs.b() ),
		integer_cast<ElementType>( a() - rhs.a() ) );
}



Color4a5& Color4a5::operator+=( Color4a5 const& rhs )
{
	*this = *this + rhs;
	return *this;
}



Color4a5& Color4a5::operator-=( Color4a5 const& rhs )
{
	*this = *this - rhs;
	return *this;
}



Color4a5 Color4a5::operator+( ElementType const& rhs ) const
{
	return Color4a5(
		integer_cast<ElementType>( r() + rhs ),
		integer_cast<ElementType>( g() + rhs ),
		integer_cast<ElementType>( b() + rhs ),
		integer_cast<ElementType>( a() + rhs ) );
}



Color4a5 Color4a5::operator-( ElementType const& rhs ) const
{
	return Color4a5(
		integer_cast<ElementType>( r() - rhs ),
		integer_cast<ElementType>( g() - rhs ),
		integer_cast<ElementType>( b() - rhs ),
		integer_cast<ElementType>( a() - rhs ) );
}



Color4a5 Color4a5::operator*( ElementType const& rhs ) const
{
	return Color4a5(
		integer_cast<ElementType>( r() * rhs ),
		integer_cast<ElementType>( g() * rhs ),
		integer_cast<ElementType>( b() * rhs ),
		integer_cast<ElementType>( a() * rhs ) );
}



Color4a5& Color4a5::operator+=( ElementType const& rhs )
{
	*this = *this + rhs;
	return *this;
}



Color4a5& Color4a5::operator-=( ElementType const& rhs )
{
	*this = *this - rhs;
	return *this;
}



Color4a5& Color4a5::operator*=( ElementType const& rhs )
{
	*this = *this * rhs;
	return *this;
}



uint16_t Color4a5::Data() const
{
	return angry_cast<uint16_t>( ( broaden_cast<uint16_t>( mHigh ) << 8u ) | mLow );
}



Color4a5::ElementType Color4a5::r() const
{
	uint8_t const raw = ( mHigh & 0b1111'1000u ) >> 3u;
	return integer_cast<uint8_t>( ( raw * details::kMaxValIn8 ) / details::kMaxValIn5 );
}



Color4a5::ElementType Color4a5::g() const
{
	uint8_t const raw = integer_cast<uint8_t>( ( Data() & 0b0000'0111'1100'0000u ) >> 6u );
	return integer_cast<uint8_t>( ( raw * details::kMaxValIn8 ) / details::kMaxValIn5 );
}



Color4a5::ElementType Color4a5::b() const
{
	uint8_t const raw = ( mLow & 0b0011'1110u ) >> 1u;
	return integer_cast<uint8_t>( ( raw * details::kMaxValIn8 ) / details::kMaxValIn5 );
}



Color4a5::ElementType Color4a5::a() const
{
	uint8_t const raw = mLow & 0b0000'0001u;
	return raw == 0u ? 0u : details::kMaxValIn8;
}

///////////////////////////////////////////////////////////////////////////////
}
