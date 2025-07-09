#pragma once
#include "math_casts.h"

#include "core_math/Limits.h"

#include "core/macros.h"

#include "rftl/cmath"
#include "rftl/limits"


namespace RF::math {
///////////////////////////////////////////////////////////////////////////////
namespace details {

// TEMPLATE QUIRK: Clang will treat constexpr call results as different when
//  evaluating them as an argument to enable_if, but MSVC will not, leading to
//  duplicate declaration errors. Both compilers work as expected when the
//  the constexpr calls are filtered through a bool_constant though. Unclear
//  what the correct standards behavior is here.

template<typename DST, typename SRC>
constexpr typename rftl::enable_if<rftl::is_floating_point<DST>::value, bool>::type could_overflow_v()
{
	return false;
}

template<typename DST, typename SRC>
constexpr typename rftl::enable_if<rftl::is_integral<DST>::value && rftl::is_floating_point<SRC>::value, bool>::type could_overflow_v()
{
	return true;
}

template<typename DST, typename SRC>
constexpr typename rftl::enable_if<rftl::is_integral<DST>::value && rftl::is_integral<SRC>::value, bool>::type could_overflow_v()
{
	constexpr SRC maxSrc = rftl::numeric_limits<SRC>::max();
	constexpr DST maxDest = rftl::numeric_limits<DST>::max();
	return //
		static_cast<DST>( maxSrc ) > maxDest ||
		maxSrc > static_cast<SRC>( maxDest );
}

template<typename DST, typename SRC>
struct could_overflow : public rftl::bool_constant<could_overflow_v<DST, SRC>()>
{ };



template<typename DST, typename SRC>
constexpr bool could_drop_sign_v()
{
	return rftl::is_signed<SRC>::value && rftl::is_unsigned<DST>::value;
}

template<typename DST, typename SRC>
struct could_drop_sign : public rftl::bool_constant<could_drop_sign_v<DST, SRC>()>
{ };



template<typename DST, typename SRC>
constexpr typename rftl::enable_if<rftl::is_unsigned<SRC>::value, bool>::type could_underflow_v()
{
	return false;
}

template<typename DST, typename SRC>
constexpr typename rftl::enable_if<rftl::is_signed<SRC>::value && rftl::is_unsigned<DST>::value, bool>::type could_underflow_v()
{
	return true;
}

template<typename DST, typename SRC>
constexpr typename rftl::enable_if<rftl::is_signed<SRC>::value && rftl::is_signed<DST>::value, bool>::type could_underflow_v()
{
	constexpr DST lowestDest = rftl::numeric_limits<DST>::lowest();
	constexpr SRC lowestSrc = rftl::numeric_limits<SRC>::lowest();
	return lowestSrc < lowestDest;
}

template<typename DST, typename SRC>
struct could_underflow : public rftl::bool_constant<could_underflow_v<DST, SRC>()>
{ };



template<typename DST, typename SRC>
constexpr typename rftl::enable_if<could_overflow<DST, SRC>::value, bool>::type will_overflow( SRC const src )
{
	constexpr DST maxDest = rftl::numeric_limits<DST>::max();
	constexpr auto lessThan = []( auto lhs, auto rhs ) -> bool {
		RF_CLANG_PUSH();
		// This can raise warnings when using high value integers and
		//  low-precision floating point (rightfully so), or different signedness
		// TODO: Consider using more specializations instead
		RF_CLANG_IGNORE( "-Wimplicit-int-float-conversion" );
		RF_CLANG_IGNORE( "-Wsign-compare" );
		RF_MSVC_INLINE_SUPPRESS( 5219 4018 4388 );
		return lhs < rhs;
		RF_CLANG_POP();
	};
	return lessThan( maxDest, src );
}

template<typename DST, typename SRC>
constexpr typename rftl::enable_if<could_overflow<DST, SRC>::value == false, bool>::type will_overflow( SRC const src )
{
	return false;
}



template<typename DST, typename SRC>
constexpr typename rftl::enable_if<could_drop_sign<DST, SRC>::value, bool>::type will_drop_sign( SRC const src )
{
	return src < 0;
}

template<typename DST, typename SRC>
constexpr typename rftl::enable_if<could_drop_sign<DST, SRC>::value == false, bool>::type will_drop_sign( SRC const src )
{
	return false;
}



template<typename DST, typename SRC>
constexpr typename rftl::enable_if<could_underflow<DST, SRC>::value && could_drop_sign<DST, SRC>::value, bool>::type will_underflow( SRC const src )
{
	return will_drop_sign<DST, SRC>( src );
}

template<typename DST, typename SRC>
constexpr typename rftl::enable_if<could_underflow<DST, SRC>::value && could_drop_sign<DST, SRC>::value == false, bool>::type will_underflow( SRC const src )
{
	constexpr DST lowestDest = rftl::numeric_limits<DST>::lowest();
	return src < lowestDest;
}

template<typename DST, typename SRC>
constexpr typename rftl::enable_if<could_underflow<DST, SRC>::value == false, bool>::type will_underflow( SRC const src )
{
	return false;
}



template<typename DST, typename SRC, typename rftl::enable_if<rftl::is_integral<SRC>::value, int>::type = 0>
void integer_cast_assert( DST const dst, SRC const src )
{
	static_assert( rftl::is_integral<DST>::value, "integral_cast only valid on integeral types" );
	RF_ASSERT( dst == static_cast<DST>( src ) );
}



template<typename DST, typename SRC, typename rftl::enable_if<rftl::is_floating_point<SRC>::value, int>::type = 0>
void integer_cast_assert( DST const dst, SRC const src )
{
	static_assert( rftl::is_integral<DST>::value, "integral_cast only valid on integeral types" );
	RF_ASSERT( static_cast<SRC>( dst ) >= src - 1 );
	RF_ASSERT( static_cast<SRC>( dst ) <= src + 1 );
}

}
///////////////////////////////////////////////////////////////////////////////

template<typename DST, typename SRC>
DST integer_cast( SRC const src )
{
	static_assert( rftl::is_integral<DST>::value, "integral_cast only valid on integeral types" );
	DST retVal = integer_truncast<DST>( src );
	details::integer_cast_assert( retVal, src );
	return retVal;
}



template<typename DST, typename SRC>
DST integer_truncast( SRC const src )
{
	static_assert( rftl::is_integral<DST>::value, "integral_truncast only valid on integeral types" );

	if( details::will_overflow<DST, SRC>( src ) )
	{
		constexpr DST maxDest = rftl::numeric_limits<DST>::max();
		return maxDest;
	}

	if( details::will_underflow<DST, SRC>( src ) )
	{
		constexpr DST lowestDest = rftl::numeric_limits<DST>::lowest();
		return lowestDest;
	}

	if constexpr( rftl::is_floating_point<SRC>::value )
	{
		if( is_quiet_NaN( src ) )
		{
			return rftl::numeric_limits<DST>::quiet_NaN();
		}

		if( is_signaling_NaN( src ) )
		{
			return rftl::numeric_limits<DST>::signaling_NaN();
		}

		if constexpr( rftl::is_same<SRC, long double>::value )
		{
			return static_cast<DST>( rftl::roundl( static_cast<long double>( src ) ) );
		}

		if constexpr( rftl::is_same<SRC, double>::value )
		{
			return static_cast<DST>( rftl::round( static_cast<double>( src ) ) );
		}

		return static_cast<DST>( rftl::roundf( static_cast<float>( src ) ) );
	}
	else
	{
		return static_cast<DST>( src );
	}
}



template<typename SRC,
	typename rftl::enable_if<rftl::is_integral<SRC>::value && rftl::is_unsigned<SRC>::value, int>::type>
constexpr SRC integer_unsigned_bitcast( SRC const src )
{
	return src;
}
constexpr uint8_t integer_unsigned_bitcast( int8_t const src )
{
	return static_cast<uint8_t>( src );
}
constexpr uint16_t integer_unsigned_bitcast( int16_t const src )
{
	return static_cast<uint16_t>( src );
}
constexpr uint32_t integer_unsigned_bitcast( int32_t const src )
{
	return static_cast<uint32_t>( src );
}
constexpr uint64_t integer_unsigned_bitcast( int64_t const src )
{
	return static_cast<uint64_t>( src );
}



template<typename DST, typename SRC>
constexpr DST float_cast( SRC const src )
{
	static_assert( rftl::is_floating_point<DST>::value, "float_cast only valid on integeral types" );
	return static_cast<DST>( src );
}



template<typename DST, typename SRC, typename rftl::enable_if<rftl::is_integral<DST>::value, int>::type>
DST real_cast( SRC const src )
{
	return integer_cast<DST>( src );
}



template<typename DST, typename SRC, typename rftl::enable_if<rftl::is_floating_point<DST>::value, int>::type>
DST real_cast( SRC const src )
{
	return float_cast<DST>( src );
}



constexpr int8_t char_integer_bitcast( char const src )
{
	static_assert( rftl::is_signed<decltype( src )>::value );
	return static_cast<int8_t>( src );
}
constexpr uint8_t char_integer_bitcast( unsigned char const src )
{
	static_assert( rftl::is_unsigned<decltype( src )>::value );
	return static_cast<uint8_t>( src );
}
constexpr uint16_t char_integer_bitcast( wchar_t const src )
{
	static_assert( rftl::is_unsigned<decltype( src )>::value );
	static_assert( sizeof( decltype( src ) ) == 2 );
	return static_cast<uint16_t>( src );
}
constexpr uint8_t char_integer_bitcast( char8_t const src )
{
	static_assert( rftl::is_unsigned<decltype( src )>::value );
	return static_cast<uint8_t>( src );
}
constexpr uint16_t char_integer_bitcast( char16_t const src )
{
	static_assert( rftl::is_unsigned<decltype( src )>::value );
	return static_cast<uint16_t>( src );
}
constexpr uint32_t char_integer_bitcast( char32_t const src )
{
	static_assert( rftl::is_unsigned<decltype( src )>::value );
	return static_cast<uint32_t>( src );
}



template<typename SRC,
	typename rftl::enable_if<rftl::is_enum<SRC>::value, int>::type>
constexpr auto enum_bitcast( SRC const src ) -> typename rftl::underlying_type<SRC>::type
{
	return static_cast<typename rftl::underlying_type<SRC>::type>( src );
}



template<typename DEST,
	typename rftl::enable_if<rftl::is_enum<DEST>::value, int>::type>
constexpr DEST enum_bitcast( typename rftl::underlying_type<DEST>::type const src )
{
	return static_cast<DEST>( src );
}

///////////////////////////////////////////////////////////////////////////////
}
