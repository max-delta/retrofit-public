#pragma once
#include "math_casts.h"

#include "core/macros.h"

#include "rftl/limits"


namespace RF {
namespace math {
///////////////////////////////////////////////////////////////////////////////

template<typename DST, typename SRC>
DST integer_cast( SRC const src )
{
	static_assert( rftl::is_integral<DST>::value, "integral_cast only valid on integeral types" );
	DST retVal = integer_truncast<DST>( src );
	RF_ASSERT( retVal == static_cast<DST>( src ) );
	return retVal;
}



template<typename DST, typename SRC>
DST integer_truncast( SRC const src )
{
	using rftl::numeric_limits;

	static_assert( rftl::is_integral<DST>::value, "integral_truncast only valid on integeral types" );

	constexpr DST maxDest = numeric_limits<DST>::max();
	constexpr DST lowestDest = numeric_limits<DST>::lowest();
	constexpr SRC lowestSrc = numeric_limits<SRC>::lowest();
	constexpr bool couldOverflow = numeric_limits<SRC>::max() > maxDest;
	RF_ACK_CONSTEXPR_SIGN_MISMATCH; // TODO: Templatize
	constexpr bool couldUnderflow = rftl::is_signed<SRC>::value ? lowestSrc < lowestDest : false;
	constexpr bool couldDropSign = rftl::is_signed<SRC>::value == true && rftl::is_signed<DST>::value == false;

	if( couldOverflow )
	{
		if( src > maxDest )
			return maxDest;
	}

	if( couldUnderflow )
	{
		RF_ACK_CONSTEXPR_SIGN_MISMATCH; // TODO: Templatize
		if( src < lowestDest )
			return lowestDest;
	}

	if( couldDropSign )
	{
		if( src < 0 )
			return lowestDest;
	}

	if( rftl::is_floating_point<SRC>::value )
	{
		return static_cast<DST>( roundf( static_cast<float>( src ) ) );
	}
	else
	{
		return static_cast<DST>( src );
	}
}



constexpr uint8_t integer_unsigned_cast( int8_t const src )
{
	return static_cast<uint8_t>( src );
}
constexpr uint16_t integer_unsigned_cast( int16_t const src )
{
	return static_cast<uint16_t>( src );
}
constexpr uint32_t integer_unsigned_cast( int32_t const src )
{
	return static_cast<uint32_t>( src );
}
constexpr uint64_t integer_unsigned_cast( int64_t const src )
{
	return static_cast<uint64_t>( src );
}
constexpr uint8_t integer_unsigned_cast( uint8_t const src )
{
	return src;
}
constexpr uint16_t integer_unsigned_cast( uint16_t const src )
{
	return src;
}
constexpr uint32_t integer_unsigned_cast( uint32_t const src )
{
	return src;
}
constexpr uint64_t integer_unsigned_cast( uint64_t const src )
{
	return src;
}



template<typename DST, typename SRC,
	typename rftl::enable_if<rftl::is_integral<DST>::value, int>::type>
DST real_cast( SRC const src )
{
	return integer_cast<DST>( src );
}



template<typename DST, typename SRC,
	typename rftl::enable_if<rftl::is_floating_point<DST>::value, int>::type>
DST real_cast( SRC const src )
{
	return static_cast<DST>( src );
}

///////////////////////////////////////////////////////////////////////////////
}}
