#pragma once
#include "rftl/cstdint"
#include "rftl/type_traits"

namespace RF::math {
///////////////////////////////////////////////////////////////////////////////

// Convert to an integer, assert if truncation occurred
template<typename DST, typename SRC>
DST integer_cast( SRC const src );

// Convert to an integer, truncate if necessary
template<typename DST, typename SRC>
DST integer_truncast( SRC const src );

// Convert to an integer in constexpr contexts
template<typename DST, typename SRC>
consteval DST integer_constcast( SRC const src );

// Reinterpret integer bits as their unsigned equivalent
template<typename SRC,
	typename rftl::enable_if<rftl::is_integral<SRC>::value && rftl::is_unsigned<SRC>::value, int>::type = 0>
constexpr SRC integer_unsigned_bitcast( SRC const src );
constexpr uint8_t integer_unsigned_bitcast( int8_t const src );
constexpr uint16_t integer_unsigned_bitcast( int16_t const src );
constexpr uint32_t integer_unsigned_bitcast( int32_t const src );
constexpr uint64_t integer_unsigned_bitcast( int64_t const src );

// Convert to a floating-point
template<typename DST, typename SRC>
constexpr DST float_cast( SRC const src );

// Convert to a real number, safely casting as needed
template<typename DST, typename SRC, //
	typename rftl::enable_if<rftl::is_integral<DST>::value, int>::type = 0>
DST real_cast( SRC const src );
template<typename DST, typename SRC, //
	typename rftl::enable_if<rftl::is_floating_point<DST>::value, int>::type = 0>
DST real_cast( SRC const src );

// Reinterpret character bits as their integer equivalent
constexpr int8_t char_integer_bitcast( char const src );
constexpr uint8_t char_integer_bitcast( unsigned char const src );
constexpr uint16_t char_integer_bitcast( wchar_t const src );
constexpr uint8_t char_integer_bitcast( char8_t const src );
constexpr uint16_t char_integer_bitcast( char16_t const src );
constexpr uint32_t char_integer_bitcast( char32_t const src );

// Just '0' + (1-9) with a check
char char_digitcast( uint8_t oneThroughNine );

// Reintepret to/from an enum and its underlying type
template<typename SRC,
	typename rftl::enable_if<rftl::is_enum<SRC>::value, int>::type = 0>
constexpr auto enum_bitcast( SRC const src ) -> typename rftl::underlying_type<SRC>::type;
template<typename DEST,
	typename rftl::enable_if<rftl::is_enum<DEST>::value, int>::type = 0>
constexpr DEST enum_bitcast( typename rftl::underlying_type<DEST>::type const src );

///////////////////////////////////////////////////////////////////////////////
}

#include "math_casts.inl"
