#pragma once
#include "core/meta/FailTemplate.h"

#include "rftl/type_traits"
#include "rftl/cstddef"


namespace RF {
///////////////////////////////////////////////////////////////////////////////

// NOTE: Nuisance casting since C++ eats types when using iostream
template<typename T>
inline constexpr int16_t iostream_cast( T in )
{
	static_assert( FailTemplate<T>(), "Bad type" );
	return {};
}

template<>
inline constexpr int16_t iostream_cast<uint8_t>( uint8_t in )
{
	return static_cast<int16_t>( in );
}

template<>
inline constexpr int16_t iostream_cast<int8_t>( int8_t in )
{
	return static_cast<int16_t>( in );
}



// Passthrough, that verifies your integer isn't going to get eaten
template<typename T>
inline constexpr T const& iostream_safecast( T const& in )
{
	static_assert( rftl::is_integral<T>::value );
	static_assert( sizeof( T ) > 1 );
	return in;
}

///////////////////////////////////////////////////////////////////////////////
}
