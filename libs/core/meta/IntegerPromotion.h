#pragma once
#include "rftl/cstddef"
#include "rftl/type_traits"

namespace RF {
///////////////////////////////////////////////////////////////////////////////

// NOTE: Nuisance static casting since C++ insists on doing integer promotion
template<typename T, typename U>
inline constexpr T angry_cast( U in )
{
	// In a sane world, types wouldn't randomly change out from underneath you,
	//  contorting signedness and sizes just to hack around old busted C-era
	//  implementation laziness
	static_assert( rftl::is_same<T, U>::value );
	return in;
}

template<>
inline constexpr uint8_t angry_cast<uint8_t, int>( int in )
{
	return static_cast<uint8_t>( in );
}

template<>
inline constexpr int8_t angry_cast<int8_t, int>( int in )
{
	return static_cast<int8_t>( in );
}

template<>
inline constexpr uint16_t angry_cast<uint16_t, int>( int in )
{
	return static_cast<uint16_t>( in );
}

template<>
inline constexpr int16_t angry_cast<int16_t, int>( int in )
{
	return static_cast<int16_t>( in );
}

///////////////////////////////////////////////////////////////////////////////
}
