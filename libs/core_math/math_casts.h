#pragma once
#include "rftl/cstdint"
#include "rftl/type_traits"

namespace RF { namespace math {
///////////////////////////////////////////////////////////////////////////////

template<typename DST, typename SRC>
DST integer_cast( SRC const src );

template<typename DST, typename SRC>
DST integer_truncast( SRC const src );

constexpr uint8_t integer_unsigned_cast( int8_t const src );
constexpr uint16_t integer_unsigned_cast( int16_t const src );
constexpr uint32_t integer_unsigned_cast( int32_t const src );
constexpr uint64_t integer_unsigned_cast( int64_t const src );
constexpr uint8_t integer_unsigned_cast( uint8_t const src );
constexpr uint16_t integer_unsigned_cast( uint16_t const src );
constexpr uint32_t integer_unsigned_cast( uint32_t const src );
constexpr uint64_t integer_unsigned_cast( uint64_t const src );

template<typename DST, typename SRC,
	typename rftl::enable_if<rftl::is_integral<DST>::value, int>::type = 0>
DST real_cast( SRC const src );
template<typename DST, typename SRC,
	typename rftl::enable_if<rftl::is_floating_point<DST>::value, int>::type = 0>
DST real_cast( SRC const src );


template<typename SRC,
	typename rftl::enable_if<rftl::is_enum<SRC>::value, int>::type = 0>
constexpr auto enum_bitcast( SRC const src ) -> typename rftl::underlying_type<SRC>::type;
template<typename DEST,
	typename rftl::enable_if<rftl::is_enum<DEST>::value, int>::type = 0>
constexpr DEST enum_bitcast( typename rftl::underlying_type<DEST>::type const src );

///////////////////////////////////////////////////////////////////////////////
}}

#include "math_casts.inl"
